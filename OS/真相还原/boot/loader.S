%include	"protectmode.inc"	; 常量, 宏, 以及一些说明
%include	"boot.inc"


PageDirBase		equ	100000h	; 页目录开始地址:	2M
PageTblBase		equ	101000h	; 页表开始地址:		2M + 4K

KERNEL_START_SECTOR equ 10
KERNEL_ENTRY_POINT  equ 0xc0001500
KERNEL_BIN_BASE_ADDR equ 0x70000
PT_NULL equ 0



org	LOADER_BASE_ADDR

	jmp	LABEL_BEGIN

[SECTION .gdt]
 ; GDT
 ;                                         段基址,        段界限     , 属性
 LABEL_GDT:			    Descriptor	           0,                 0, 0							; 空描述符
;  LABEL_DESC_NORMAL: 	Descriptor	           0,            0FFFFFh, DA_DRWA + DA_32			; Normal 描述符
 LABEL_DESC_NORMAL:		Descriptor 			   0, 			 0FFFFFh, DA_DRWA + DA_LIMIT_4K		; Normal Descriptor
 LABEL_DESC_PAGE_DIR:	Descriptor   PageDirBase,               4095, DA_DRW					; 页目录描述符
 LABEL_DESC_PAGE_TBL:	Descriptor   PageTblBase,       4096 * 8 - 1, DA_DRW					; 页表描述符
;  LABEL_DESC_PAGE_DIR:	Descriptor   PageDirBase,             0FFFFh, DA_DRW					; 页目录描述符
;  LABEL_DESC_PAGE_TBL:	Descriptor   PageTblBase,             0FFFFh, DA_DRW					; 页表描述符
;  LABEL_DESC_CODE32:  	Descriptor	           0,   SegCode32Len - 1, DA_C + DA_32				; 非一致代码段, 32
 LABEL_DESC_CODE32:		Descriptor	           0,            0FFFFFh, DA_C + DA_LIMIT_4K + DA_32; 非一致32位代码段
;  LABEL_DESC_DATA:		Descriptor	           0,        DataLen - 1, DA_DRW					; Data
 LABEL_DESC_DATA:		Descriptor	           0,             0FFFFh, DA_DRW 		 			; Data
 LABEL_DESC_DATA2:		Descriptor	           0,            0FFFFFh, DA_DRW + DA_LIMIT_4K		;Data_For_Kernel
 LABEL_DESC_STACK:  	Descriptor	           0,            0FFFFFh, DA_DRWA+DA_32+DA_LIMIT_4K  ; Stack, 32 位
;  LABEL_DESC_STACK:..	Descriptor	           0,        TopOfStack, DA_DRWA + DA_32			; Stack, 32 位
 LABEL_DESC_VIDEO:		Descriptor	     0B8000h,            0FFFFh, DA_DRW						; 显存首地址
 ; GDT 结束

 GdtLen		equ	$ - LABEL_GDT	; GDT长度
 GdtPtr		dw	GdtLen - 1		; GDT界限
 		dd	0					; GDT基地址

 ; GDT 选择子
 SelectorNormal		equ	LABEL_DESC_NORMAL	- LABEL_GDT
 SelectorPageDir	equ	LABEL_DESC_PAGE_DIR	- LABEL_GDT
 SelectorPageTbl	equ	LABEL_DESC_PAGE_TBL	- LABEL_GDT
 SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
 SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT
 SelectorData2		equ	LABEL_DESC_DATA2	- LABEL_GDT
 SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT
 SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT

; END of [SECTION .gdt]

[SECTION .data1]	 ; 数据段
 ALIGN	32
 [bits	32]
 LABEL_DATA:
 ; 实模式下使用这些符号
 ; 字符串
 _szPMMessage:			db	"In Protect Mode now. ^-^", 0Ah, 0Ah, 0	; 进入保护模式后显示此字符串
 _szMemChkTitle:		db	"BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0	; 进入保护模式后显示此字符串
 _szRAMSize				db	"RAM size:", 0
 _szReturn				db	0Ah, 0
 ; 变量
 _wSPValueInRealMode	dw	0
 _dwMCRNumber:			dd	0	; Memory Check Result
 _dwDispPos:			dd	(80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列。
 _dwMemSize:			dd	0
 _ARDStruct:			; Address Range Descriptor Structure 地址范围描述符结构体
 	_dwBaseAddrLow:		dd	0
 	_dwBaseAddrHigh:	dd	0
 	_dwLengthLow:		dd	0
 	_dwLengthHigh:		dd	0
 	_dwType:			dd	0
 
 _MemChkBuf:				times	256	db	0
 
 ; 保护模式下使用这些符号
 szPMMessage				equ	_szPMMessage	- $$
 szMemChkTitle				equ	_szMemChkTitle	- $$
 szRAMSize				equ	_szRAMSize	- $$
 szReturn				equ	_szReturn	- $$
 dwDispPos				equ	_dwDispPos	- $$
 dwMemSize				equ	_dwMemSize	- $$
 dwMCRNumber				equ	_dwMCRNumber	- $$
 ARDStruct				equ	_ARDStruct	- $$
 dwBaseAddrLow				equ	_dwBaseAddrLow	- $$
 dwBaseAddrHigh				equ	_dwBaseAddrHigh	- $$
 dwLengthLow				equ	_dwLengthLow	- $$
 dwLengthHigh				equ	_dwLengthHigh	- $$
 dwType					equ	_dwType		- $$
 MemChkBuf				equ	_MemChkBuf	- $$
 
 DataLen				equ	$ - LABEL_DATA
; END of [SECTION .data1]
 

; ; 全局堆栈段
; [SECTION .gs]
; ALIGN	32
; [bits	32]
; LABEL_STACK:
; 	times 1024 db 0

; TopOfStack	equ	$ - LABEL_STACK - 1

; ; END of [SECTION .gs]


[SECTION .s16]
[bits	16]
LABEL_BEGIN:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h

	mov	[_wSPValueInRealMode], sp

	; 得到内存数 begin 
	mov	ebx, 0
	mov	di, _MemChkBuf ; _MemChkBuf: times	256	db	0, 缓冲区
.loop:
	mov	eax, 0E820h
	mov	ecx, 20
	mov	edx, 0534D4150h
	int	15h				; 每次中断，都把内存数据(不同内存段的描述和大小) copy 到 缓冲区_MemChkBuf中，
	jc	LABEL_MEM_CHK_FAIL ; jump if carry(CF=1), cf=0表示读取内存信息没有错误
	add	di, 20	; es:di 指向一个地址范围描述符结构 ARDS
	inc	dword [_dwMCRNumber]; _dwMCRNumber: dd	0	; Memory Check Result
	cmp	ebx, 0	; ebx 存储着下一个地址描述符所需要的后续值;
	jne	.loop
	jmp	LABEL_MEM_CHK_OK
LABEL_MEM_CHK_FAIL:
	mov	dword [_dwMCRNumber], 0
LABEL_MEM_CHK_OK:
	; 得到内存数 over
	
	;; 初始化 32 位代码段描述符
	; xor	eax, eax
	; mov	ax, cs
	; shl	eax, 4
	; add	eax, LABEL_SEG_CODE32
	; mov	word [LABEL_DESC_CODE32 + 2], ax
	; shr	eax, 16
	; mov	byte [LABEL_DESC_CODE32 + 4], al
	; mov	byte [LABEL_DESC_CODE32 + 7], ah

	; 初始化数据段描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_DATA
	mov	word [LABEL_DESC_DATA + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_DATA + 4], al
	mov	byte [LABEL_DESC_DATA + 7], ah

	; 初始化堆栈段描述符
	; xor	eax, eax
	; mov	ax, ds
	; shl	eax, 4
	; add	eax, LABEL_STACK
	; mov	word [LABEL_DESC_STACK + 2], ax
	; shr	eax, 16
	; mov	byte [LABEL_DESC_STACK + 4], al
	; mov	byte [LABEL_DESC_STACK + 7], ah
	
	

	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds	; 这里为什么要将段选择子设置成0
	shl	eax, 4
	add	eax, LABEL_GDT		; eax <- gdt 基地址
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 加载 GDTR
	lgdt	[GdtPtr]

	; 关中断
	cli
	; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax
	; 真正进入保护模式
	jmp	dword SelectorCode32:LABEL_SEG_CODE32	; 执行这一句会把 SelectorCode32 装入 cs, 并跳转到 Code32Selector:0  处

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[SECTION .s32]; 32 位代码段. 由实模式跳入.
[bits	32]
LABEL_SEG_CODE32:

	mov	ax, SelectorData
	mov	ds, ax			; 数据段选择子
	mov	ax, SelectorData
	mov	es, ax
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子

	mov	ax, SelectorStack
	mov	ss, ax			; 堆栈段选择子

	; mov	esp, TopOfStack
	mov esp, LOADER_BASE_ADDR
	; 下面显示一个字符串
	; _szPMMessage:db "In Protect Mode now. ^-^", 0Ah, 0Ah, 0 ; 进入保护模式后显示此字符串
	; szPMMessage equ _szPMMessage - $$ 
	; $$ == LABEL_DATA
	push	szPMMessage
	call	DispStr
	add	esp, 4

	push	szMemChkTitle ; szMemChkTitle equ	_szMemChkTitle	- $$, _szMemChkTitle: db "BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0	; 进入保护模式后显示此字符串
	call	DispStr
	add	esp, 4
	call	DispMemSize		; 显示内存信息
	mov ax, SelectorData2
	mov ds, ax	
	mov eax, KERNEL_START_SECTOR        ; kernel.bin所在的扇区号
	mov ebx, KERNEL_BIN_BASE_ADDR       ; 从磁盘读出后，写入到ebx指定的地址
	mov ecx, 200			       ; 读入的扇区数

	call rd_disk_m_32


	mov	ax, SelectorData
	mov	ds, ax			; 数据段选择子
	call	SetupPaging		; 启动分页机制


	; 在函数里面改栈就错了 ret 不回来
	; push eax
	; mov eax, [esp+0xc0000000]
	; jmp dword SelectorCode32:.enter_kernel	  ;为了更新流水线,但是这里为什么会跳到其他地址?
.enter_kernel:
	mov esp,0xc009f000
	; push	dword [dwMemSize]
	mov ax, SelectorData2
	mov ds, ax
	mov es, ax
	call init_kernel
	; mov ax, SelectorData2
	; mov ds, ax
	; xchg bx,bx
	jmp KERNEL_ENTRY_POINT

;初始化内核
init_kernel:
	xor eax, eax
	xor ebx, ebx		;ebx记录程序头表地址
	xor ecx, ecx		;cx记录程序头表中的program header数量
	xor edx, edx		;dx 记录program header尺寸,即e_phentsize
	mov dx,[KERNEL_BIN_BASE_ADDR+42];一个program的大小
	mov ebx,[KERNEL_BIN_BASE_ADDR+28]; 偏移文件开始部分28字节的地方是e_phoff,表示第1 个program header在文件中的偏移量
	add ebx,KERNEL_BIN_BASE_ADDR
	mov cx,[KERNEL_BIN_BASE_ADDR+44];表项数目
.each_segment:
	cmp byte [ebx+0],PT_NULL	;p_type 等于 NULL
	; cmp byte [ebx+10],PT_NULL ;6，错误代码
	je .PTNULL
	push dword [ebx+16];表项的尺寸
	mov eax,[ebx+4];表项距离文件头的偏移量
	add eax,KERNEL_BIN_BASE_ADDR;
	push eax;表段的起始地址,源地址
	push dword[ebx+8];在编译时生成的程序被加载的目的地址
	call mem_cpy
	add esp,12;将之前传入的参数丢弃
.PTNULL:
	add ebx,edx
	loop .each_segment
	ret
mem_cpy:
	cld 
	push ebp
	mov ebp,esp
	push ecx
	mov edi, [ebp + 8]	   ; dst
	mov esi, [ebp + 12]	   ; src
	mov ecx, [ebp + 16]	   ; size
	rep movsb			   ; 逐字节拷贝	  ;问题1: 越界 limit
								;问题2: 内核栈弄失误,难以映射高地址
	pop ecx
	pop ebp
	ret

; 启动分页机制 --------------------------------------------------------------
SetupPaging:
	; 首先初始化页目录
	mov	ax, SelectorPageDir	; 此段首地址为 PageDirBase
	mov	es, ax				; es 现在用的是页目录表的段选择子
	xor	edi, edi
	xor	eax, eax
	; 此时eax中存储的是第一个页表的地址
	mov	eax, PageTblBase | PG_P  | PG_USU | PG_RWW
	stosd				; 这里用到了 es:edi
	; 0xc00 以上的目录项属于内核空间
	mov	edi,0xc00		; 为了简化, 所有页表在内存中是连续的.
	stosd
	mov	eax, PageDirBase | PG_P  | PG_USU | PG_RWW
	mov edi, 4092
	stosd				; 使最后一个目录项指向页目录表自己的地址

	; 低 1 MB 的页表项
	mov	ax, SelectorPageTbl	; 此段首地址为 PageTblBase
	mov	es, ax
	mov	ecx,256		; 1MB 低端内存 / 4KB = 256
	mov edi,0
	mov	eax, PG_P  | PG_USU | PG_RWW
.1:
	stosd
	add	eax, 4096
	loop	.1

;	高 1 GB 内核页表的PDE
	mov	ax, SelectorPageDir	; 此段首地址为 PageDirBase
	mov	es, ax				; es 现在用的是页目录表的段选择子	mov eax, PageTblBase
	mov eax, PageTblBase
	or  eax, PG_P  | PG_USU | PG_RWW
	add eax, 0x1000
	mov ecx, 254
	mov edi,0xc00 + 4 

.2:
	stosd	; es: edi
	add eax,0x1000
	loop .2
	
	; 存储到原来 gdt 所有位置
	sgdt	[GdtPtr]
	
	; 修改VIDEO段基址
	mov ax,SelectorNormal
	mov ds,ax
	mov ebx,LABEL_GDT
	or dword [ebx + 0x40 + 4],0xc0000000
	add dword [GdtPtr + 2],0xc0000000
	add esp,0xc0000000
	mov	ax, SelectorData
	mov	ds, ax			; 数据段选择子
	mov	eax, PageDirBase
	mov	cr3, eax
	mov	eax, cr0
	or	eax, 0x80000000
	mov	cr0, eax
	lgdt	[GdtPtr]

	nop

	ret

; 分页机制启动完毕 ----------------------------------------------------------


; call	DispMemSize; 由保护模式的 显示内存信息 跳转到这里
DispMemSize:
	push	esi
	push	edi
	push	ecx
	
; _MemChkBuf: times	256	db	0
; MemChkBuf equ	_MemChkBuf	- $$
; $$ == LABEL_DATA
	mov	esi, MemChkBuf ; 他就是 存放内存描述符结构体 的 缓冲区，共256个字节，每个结构体=20字节，所以最多存放12个结构体
	
; _ARDStruct:			; Address Range Descriptor Structure 地址范围描述符结构体
;	_dwBaseAddrLow:		dd	0 基地址低32位
;	_dwBaseAddrHigh:	dd	0 基地址高32位
;	_dwLengthLow:		dd	0 长度的低32位
;	_dwLengthHigh:		dd	0 长度的高32位
;	_dwType:			dd	0 这个地址范围的地址类型

	mov	ecx, [dwMCRNumber]; 外循环 for(int i=0;i<[MCRNumber];i++)//每次得到一个ARDS
	
.loop:				  ;{ loop 的循环数 == ecx ，上行代码已经赋值
	mov	edx, 5		  ;  内循环 for(int j=0;j<5;j++) //每次得到一个ARDS中的成员
	mov	edi, ARDStruct	  ;  {//依次显示BaseAddrLow,BaseAddrHigh,LengthLow,
	
.1:				  ;             LengthHigh,Type
	push	dword [esi]
	call	DispInt		  ; DispInt(MemChkBuf[j*4]); // 显示一个成员
	pop	eax		  		  ; 将偏移地址 弹出到 eax

	stosd			 	  ; ARDStruct[j*4] = MemChkBuf[j*4]; 将 eax copy到 es:edi 指向的目的地址(ARDStruct)
						  ; 为什么要吧内存段描述符结构体从缓冲区copy到ARDS暂存呢？因为下面代码要用到变量：dwType + dwBaseAddrLow + dwLengthLow
	add	esi, 4		  	  ; mov	esi, MemChkBuf (256个0的内存空间的偏移地址)
	dec	edx		 		  ; edx == 总循环次数(5--)
	cmp	edx, 0		  	  ;
	jnz	.1		  		  ;  } 就这样循环下去吧，直到为0为止；
	
	call	DispReturn	  ;  printf("\n");
	
	cmp	dword [dwType], 1
	jne	.2		  ;  {	  ;  如果该内存不可用的话，跳转到 .2;
	mov	eax, [dwBaseAddrLow]; 基地址的低32位
	add	eax, [dwLengthLow]  ; 长度的低32位
	cmp	eax, [dwMemSize]  ;    if(BaseAddrLow + LengthLow > MemSize-内存大小),MemSize初始化为0；求最大内存范围值
	jb	.2		  		  ; jb：无符号小于则跳转
						  ; _dwMemSize:				dd	0
						  ; dwMemSize				equ	_dwMemSize	- $$
	mov	[dwMemSize], eax  ;    MemSize = BaseAddrLow + LengthLow;
.2:				  ;  } ; Type==AddressRangeReserved (该段内存不可用的话)
	loop	.loop		  ;}
				  ;
	call	DispReturn	  ;printf("\n");
	
	push	szRAMSize	  ; _szRAMSize				db	"RAM size:", 0 ;字符串以 0 结尾
	call	DispStr		  ;printf("RAM size:");
	add	esp, 4		  ;

	push	dword [dwMemSize]
	call	DispInt		  ;DispInt(MemSize);
	add	esp, 4		  ;

	pop	ecx
	pop	edi
	pop	esi
	ret

%include	"lib.inc"	; 显示函数

;-------------------------------------------------------------------------------
			   ;功能:读取硬盘n个扇区
rd_disk_m_32:	   
;-------------------------------------------------------------------------------
							 ; eax=LBA扇区号
							 ; ebx=将数据写入的内存地址
							 ; ecx=读入的扇区数
      mov esi,eax	   ; 备份eax
      mov di,cx		   ; 备份扇区数到di
;读写硬盘:
;第1步：设置要读取的扇区数
	mov dx,0x1f2
	mov al,cl
	out dx,al            ;读取的扇区数
	mov eax,esi	   ;恢复ax

;第2步：将LBA地址存入0x1f3 ~ 0x1f6

	;LBA地址7~0位写入端口0x1f3
	mov dx,0x1f3                       
	out dx,al                          

	;LBA地址15~8位写入端口0x1f4
	mov cl,8
	shr eax,cl
	mov dx,0x1f4
	out dx,al
	;LBA地址23~16位写入端口0x1f5
	shr eax,cl
	mov dx,0x1f5
	out dx,al
	shr eax,cl
	and al,0x0f	   ;lba第24~27位
	or al,0xe0	   ; 设置7～4位为1110,表示lba模式
	mov dx,0x1f6
	out dx,al

	;第3步：向0x1f7端口写入读命令，0x20 
	mov dx,0x1f7
	mov al,0x20                        
	out dx,al

;;;;;;; 至此,硬盘控制器便从指定的lba地址(eax)处,读出连续的cx个扇区,下面检查硬盘状态,不忙就能把这cx个扇区的数据读出来

;第4步：检测硬盘状态
  .not_ready:		   ;测试0x1f7端口(status寄存器)的的BSY位
	;同一端口,写时表示写入命令字,读时表示读入硬盘状态
	nop
	in al,dx
	and al,0x88	   ;第4位为1表示硬盘控制器已准备好数据传输,第7位为1表示硬盘忙
	cmp al,0x08
	jnz .not_ready	   ;若未准备好,继续等。

;第5步：从0x1f0端口读数据
	mov ax, di	   ;以下从硬盘端口读数据用insw指令更快捷,不过尽可能多的演示命令使用,
			   ;在此先用这种方法,在后面内容会用到insw和outsw等

	mov dx, 256	   ;di为要读取的扇区数,一个扇区有512字节,每次读入一个字,共需di*512/2次,所以di*256
	mul dx
	mov cx, ax	   
	mov dx, 0x1f0
  .go_on_read:
	in ax,dx		
				; ds没设置好
	mov [ebx], ax
	add ebx, 2

	loop .go_on_read
	ret

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]