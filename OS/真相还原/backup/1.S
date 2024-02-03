 ; GDT
 ;                                         段基址,        段界限     , 属性
 LABEL_GDT:			    Descriptor	           0,                 0, 0						; 空描述符
 LABEL_DESC_NORMAL:		Descriptor	           0,           0FFFFFh, DA_DRWA + DA_LIMIT_4K	; Normal 描述符
;  LABEL_DESC_NORMAL:	Descriptor 			   0, 		0FFFFFh, DA_DRWA + DA_32		; Normal Descriptor
 LABEL_DESC_PAGE_DIR:	Descriptor   PageDirBase,              4095, DA_DRW					; 页目录描述符
 LABEL_DESC_PAGE_TBL:	Descriptor   PageTblBase,      4096 * 8 - 1, DA_DRW					; 页表描述符
;  LABEL_DESC_PAGE_DIR:	Descriptor   PageDirBase,            0ffffh, DA_DRW					; 页目录描述符
;  LABEL_DESC_PAGE_TBL:	Descriptor   PageTblBase,            0ffffh, DA_DRW					; 页表描述符
;  LABEL_DESC_CODE32:	Descriptor	           0,  SegCode32Len - 1, DA_C + DA_32			; 非一致代码段
 LABEL_DESC_CODE32:		Descriptor	           0,           0FFFFFh, DA_C + DA_32		    ; 非一致代码段
;  LABEL_DESC_DATA:		Descriptor	           0,	    DataLen - 1, DA_DRW					; Data,用于 loader 局部变量
 LABEL_DESC_DATA:		Descriptor	           0,           0FFFFFh, DA_DRW					; Data,用于 loader
 LABEL_DESC_DATA2:		Descriptor	           0,           0FFFFFh, DA_DRW + DA_LIMIT_4K	; Data_For_Kernel
;  LABEL_DESC_STACK:	Descriptor	           0,        0ffffh, DA_DRWA + DA_32		; Stack, 32 位
 LABEL_DESC_STACK:		Descriptor	           0,        TopOfStack, DA_DRWA + DA_32		; Stack, 32 位
 LABEL_DESC_VIDEO:		Descriptor	     0B8000h,           0FFFFFh, DA_DRW	+ DA_LIMIT_4K	; 显存首地址
 ; GDT 结束