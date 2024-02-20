#include "interrupt.h"
#include "print.h"
#include "stdint.h"
#include "descriptor.h"
#include "io.h"


#define PIC_M_CTRL 0x20	       // 这里用的可编程中断控制器是8259A,主片的控制端口是0x20
#define PIC_M_DATA 0x21	       // 主片的数据端口是0x21
#define PIC_S_CTRL 0xa0	       // 从片的控制端口是0xa0
#define PIC_S_DATA 0xa1	       // 从片的数据端口是0xa1
#define IDT_DESC_CNT 0x21      // 目前总共支持的中断数

#define GET_IF(EFLAG_VAR)  __asm__ __volatile__("pushfl; pop %0" : "=g" (EFLAG_VAR))
#define IF_FLAG 0x200


/*中断门描述符*/
struct gate_desc {
        uint16_t    func_offset_low_word;    // 低16位置偏移
        uint16_t    selector;    // 中断处理程序目标代码段描述符选择子
        uint8_t     dcount;      //占位，全是0
        uint8_t     attribute;   // P DPL S TYPE 等属性
        uint16_t    func_offset_high_word;   // 高16位置偏移
};

char* intr_name[IDT_DESC_CNT];              // 用于保存异常的名字
static struct gate_desc idt[IDT_DESC_CNT];  // 中断描述符表
intr_handler idt_table[IDT_DESC_CNT];   //中断处理程序数组
extern intr_handler intr_entry_table[IDT_DESC_CNT];     // 引用中断处理程序入口地址数组


// 初始化中断描述符
static void make_idt_desc(struct gate_desc * desc,uint8_t attr,intr_handler func)
{
        desc->func_offset_low_word = (uint32_t)func & 0x0000FFFF;
        desc->func_offset_high_word = ((uint32_t)func & 0xFFFF0000) >> 16;
        desc->attribute = attr;
        desc->dcount = 0;
        desc->selector = SELECTOR_K_CODE;
}

// 初始化中断描述符表
static void idt_desc_init(void)
{
        for (int i = 0; i <IDT_DESC_CNT; i++) {
                make_idt_desc(&idt[i],IDT_DESC_ATTR_DPL0,intr_entry_table[i]);
        }
        put_str("idt_desc_init done \n");
}

// 通用的中断处理函数,一般用在异常出现时的处理
static void general_intr_handler(uint8_t vec_nr)
{
        if (vec_nr == 0x27 || vec_nr == 0x2f) {	// 0x2f是从片8259A上的最后一个irq引脚，保留
           return;		//IRQ7和IRQ15会产生伪中断(spurious interrupt),无须处理。
        }
        put_str("int vector: 0x");
        put_int(vec_nr);
        put_char('\n');
}

// 完成一般中断处理函数注册及异常名称注册
static void register_interrupts_and_exceptions(void)
{
        for (int i = 0; i < IDT_DESC_CNT; i++) {
        idt_table[i] = general_intr_handler;
        intr_name[i] = "unknown";       // 先统一赋值为unknown 
        }
        intr_name[0] = "#DE Divide Error";
        intr_name[1] = "#DB Debug Exception";
        intr_name[2] = "NMI Interrupt";
        intr_name[3] = "#BP Breakpoint Exception";
        intr_name[4] = "#OF Overflow Exception";
        intr_name[5] = "#BR BOUND Range Exceeded Exception";
        intr_name[6] = "#UD Invalid Opcode Exception";
        intr_name[7] = "#NM Device Not Available Exception";
        intr_name[8] = "#DF Double Fault Exception";
        intr_name[9] = "Coprocessor Segment Overrun";
        intr_name[10] = "#TS Invalid TSS Exception";
        intr_name[11] = "#NP Segment Not Present";
        intr_name[12] = "#SS Stack Fault Exception";
        intr_name[13] = "#GP General Protection Exception";
        intr_name[14] = "#PF Page-Fault Exception";
        // intr_name[15] 第15项是intel保留项，未使用
        intr_name[16] = "#MF x87 FPU Floating-Point Error";
        intr_name[17] = "#AC Alignment Check Exception";
        intr_name[18] = "#MC Machine-Check Exception";
        intr_name[19] = "#XF SIMD Floating-Point Exception";
}

// 初始化 8259A
static void pic_init(void) {
        // 初始化主片
        outb (PIC_M_CTRL, 0x11);   // ICW1: 边沿触发,级联8259.
        outb (PIC_M_DATA, 0x20);   // ICW2: 起始中断向量号为0x20,也就是IR[0-7] 为 0x20 ~ 0x27.
        outb (PIC_M_DATA, 0x04);   // ICW3: IR2接从片. 
        outb (PIC_M_DATA, 0x01);   // ICW4: 8086模式, 正常EOI
        // 初始化从片
        outb (PIC_S_CTRL, 0x11);    // ICW1: 边沿触发,级联8259.
        outb (PIC_S_DATA, 0x28);    // ICW2: 起始中断向量号为0x28,也就是IR[8-15] 为 0x28 ~ 0x2F.
        outb (PIC_S_DATA, 0x02);    // ICW3: 设置从片连接到主片的IR2引脚
        outb (PIC_S_DATA, 0x01);    // ICW4: 8086模式, 正常EOI
        // 打开主片上IR0,也就是目前只接受时钟产生的中断
        outb (PIC_M_DATA, 0xfe);
        outb (PIC_S_DATA, 0xff);
        put_str("pic_init done\n");
}

// 中断所有的初始化工作
void idt_init(void) 
{
        put_str("idt_init\n");
        idt_desc_init();
        register_interrupts_and_exceptions();
        pic_init();
        // 加载idt
        uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)(uint32_t)idt << 16));
        asm volatile("lidt %0" : : "m" (idt_operand));
        put_str("idt_init done\n");
}

// 获取intr状态
enum intr_status get_intr_status(void) {
        uint32_t flags;
        GET_IF(flags);
        return (flags & IF_FLAG) ? intr_on : intr_off;
}

// 设置intr状态,并且返回设置后的状态（防止设置失败，多一道检查）
enum intr_status set_intr_status(enum intr_status status) {
        if(status == intr_on) {
        // 开启中断
        __asm__ __volatile__("sti");
        }else {
        // 关闭中断
        __asm__ __volatile__("cli");
        }
        return get_intr_status();
}