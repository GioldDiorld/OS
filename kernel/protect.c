
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              protect.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "fs.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "string.h"
#include "global.h"
#include "proto.h"
#include "page.h"
#include "hd.h"


/* 本文件内函数声明 */
PRIVATE void init_idt_desc(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege);


/* 中断处理函数 */
void	divide_error();
void	single_step_exception();
void	nmi();
void	breakpoint_exception();
void	overflow();
void	bounds_check();
void	inval_opcode();
void	copr_not_available();
void	double_fault();
void	copr_seg_overrun();
void	inval_tss();
void	segment_not_present();
void	stack_exception();
void	general_protection();
void	page_fault();
void	copr_error();
void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();


/*======================================================================*
                            init_prot
 *----------------------------------------------------------------------*
 初始化 IDT
 *======================================================================*/
PUBLIC void init_prot()
{
	init_8259A();

	/* 全部初始化成中断门(没有陷阱门) */
	init_idt_desc(INT_VECTOR_DIVIDE,	DA_386IGate,
		      divide_error,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_DEBUG,		DA_386IGate,
		      single_step_exception,	PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_NMI,		DA_386IGate,
		      nmi,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_BREAKPOINT,	DA_386IGate,
		      breakpoint_exception,	PRIVILEGE_USER);

	init_idt_desc(INT_VECTOR_OVERFLOW,	DA_386IGate,
		      overflow,			PRIVILEGE_USER);

	init_idt_desc(INT_VECTOR_BOUNDS,	DA_386IGate,
		      bounds_check,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_INVAL_OP,	DA_386IGate,
		      inval_opcode,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_COPROC_NOT,	DA_386IGate,
		      copr_not_available,	PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_DOUBLE_FAULT,	DA_386IGate,
		      double_fault,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_COPROC_SEG,	DA_386IGate,
		      copr_seg_overrun,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_INVAL_TSS,	DA_386IGate,
		      inval_tss,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_SEG_NOT,	DA_386IGate,
		      segment_not_present,	PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_STACK_FAULT,	DA_386IGate,
		      stack_exception,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_PROTECTION,	DA_386IGate,
		      general_protection,	PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_PAGE_FAULT,	DA_386IGate,
		      page_fault,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_COPROC_ERR,	DA_386IGate,
		      copr_error,		PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ0 + 0,      DA_386IGate,
                      hwint00,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ0 + 1,      DA_386IGate,
                      hwint01,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ0 + 2,      DA_386IGate,
                      hwint02,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ0 + 3,      DA_386IGate,
                      hwint03,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ0 + 4,      DA_386IGate,
                      hwint04,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ0 + 5,      DA_386IGate,
                      hwint05,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ0 + 6,      DA_386IGate,
                      hwint06,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ0 + 7,      DA_386IGate,
                      hwint07,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ8 + 0,      DA_386IGate,
                      hwint08,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ8 + 1,      DA_386IGate,
                      hwint09,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ8 + 2,      DA_386IGate,
                      hwint10,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ8 + 3,      DA_386IGate,
                      hwint11,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ8 + 4,      DA_386IGate,
                      hwint12,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ8 + 5,      DA_386IGate,
                      hwint13,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ8 + 6,      DA_386IGate,
                      hwint14,                  PRIVILEGE_KRNL);

        init_idt_desc(INT_VECTOR_IRQ8 + 7,      DA_386IGate,
                      hwint15,                  PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_SYS_CALL,	DA_386IGate,
		      sys_call,			PRIVILEGE_USER);

	/* Fill the TSS descriptor in GDT */
	memset(&tss, 0, sizeof(tss));
	tss.ss0	= SELECTOR_KERNEL_DS;
	init_desc(&gdt[INDEX_TSS],
		  makelinear(SELECTOR_KERNEL_DS, &tss),
		  sizeof(tss) - 1,
		  DA_386TSS);
	tss.iobase = sizeof(tss); /* No IO permission bitmap */

	/* Fill the LDT descriptors of each proc in GDT  */
	int i;
	for (i = 0; i < NR_TASKS + NR_PROCS; i++) {
		memset(&proc_table[i], 0, sizeof(struct proc));

		proc_table[i].ldt_sel = SELECTOR_LDT_FIRST + (i << 3);
		assert(INDEX_LDT_FIRST + i < GDT_SIZE);
		init_desc(&gdt[INDEX_LDT_FIRST + i],
			  makelinear(SELECTOR_KERNEL_DS, proc_table[i].ldts),
			  LDT_SIZE * sizeof(struct descriptor) - 1,
			  DA_LDT);
	}
}


/*======================================================================*
                             init_idt_desc
 *----------------------------------------------------------------------*
 初始化 386 中断门
 *======================================================================*/
PUBLIC void init_idt_desc(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege)
{
	struct gate * p_gate	= &idt[vector];
	u32 base = (u32)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= SELECTOR_KERNEL_CS;
	p_gate->dcount		= 0;
	p_gate->attr		= desc_type | (privilege << 5);
	p_gate->offset_high	= (base >> 16) & 0xFFFF;
}


/*======================================================================*
                           seg2phys
 *----------------------------------------------------------------------*
 由段名求绝对地址
 *======================================================================*/
PUBLIC u32 seg2linear(u16 seg)
{
	struct descriptor* p_dest = &gdt[seg >> 3];

	return (p_dest->base_high << 24) | (p_dest->base_mid << 16) | (p_dest->base_low);
}

/*======================================================================*
                           init_descriptor
 *----------------------------------------------------------------------*
 初始化段描述符
 *======================================================================*/
PUBLIC void init_desc(struct descriptor * p_desc, u32 base, u32 limit, u16 attribute)
{
	p_desc->limit_low	= limit & 0x0FFFF;		/* 段界限 1		(2 字节) */
	p_desc->base_low	= base & 0x0FFFF;		/* 段基址 1		(2 字节) */
	p_desc->base_mid	= (base >> 16) & 0x0FF;		/* 段基址 2		(1 字节) */
	p_desc->attr1		= attribute & 0xFF;		/* 属性 1 */
	p_desc->limit_high_attr2= ((limit >> 16) & 0x0F) |
				  ((attribute >> 8) & 0xF0);	/* 段界限 2 + 属性 2 */
	p_desc->base_high	= (base >> 24) & 0x0FF;		/* 段基址 3		(1 字节) */
}

/*======================================================================*
                            exception_handler
 *----------------------------------------------------------------------*
 异常处理
 *======================================================================*/
PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
	int i;
	int text_color = 0x74; /* 灰底红字 */
	char err_description[][64] = {	"#DE Divide Error",
					"#DB RESERVED",
					"—  NMI Interrupt",
					"#BP Breakpoint",
					"#OF Overflow",
					"#BR BOUND Range Exceeded",
					"#UD Invalid Opcode (Undefined Opcode)",
					"#NM Device Not Available (No Math Coprocessor)",
					"#DF Double Fault",
					"    Coprocessor Segment Overrun (reserved)",
					"#TS Invalid TSS",
					"#NP Segment Not Present",
					"#SS Stack-Segment Fault",
					"#GP General Protection",
					"#PF Page Fault",
					"—  (Intel reserved. Do not use.)",
					"#MF x87 FPU Floating-Point Error (Math Fault)",
					"#AC Alignment Check",
					"#MC Machine Check",
					"#XF SIMD Floating-Point Exception"
				};

	/* 通过打印空格的方式清空屏幕的前五行，并把 disp_pos 清零 */
	disp_pos = 0;
	for(i=0;i<80*5;i++){
		disp_str(" ");
	}
	disp_pos = 0;

	disp_color_str("Exception! --> ", text_color);
	disp_color_str(err_description[vec_no], text_color);
	disp_color_str("\n\n", text_color);
	disp_color_str("EFLAGS:", text_color);
	disp_int(eflags);
	disp_color_str("CS:", text_color);
	disp_int(cs);
	disp_color_str("EIP:", text_color);
	disp_int(eip);

	if(err_code != 0xFFFFFFFF){
		disp_color_str("Error code:", text_color);
		disp_int(err_code);
	}
}

PUBLIC void page_fault_handler()
{
    unsigned int address;
    /* get the address */
    __asm__("movl %%cr2,%0":"=r" (address));

    if (address > MAX_VISUAL_MEMORY) {
        panic("page fault in kernel mode, addr %08x\n",address);
    }

    int nr = ADDR2PAGE(address);

    // try to find a free page
	int i;
    for (i = 0; i < MAX_PAGE_NUM; i++) {
        if (!get_bit(phys_page_map, i)) {
			// printl("pfh%d ", i);
            set_bit(phys_page_map, i);
            PAGE2PAGE_TABLE(nr) = PAGE2ADDR(i) | PG_USU | PG_RWW | PG_P;
            // printl("{MM}get free phys page: %x\n", pa);
            if (get_bit(swap_map, nr))
            {
                swap_rw(nr, address, ATA_READ);
                clear_bit(swap_map, nr);
            }
            return;
        }
    }

    // no free page left. swap out the first visual page in visual page queue and clear its appearance in page table
	unsigned int va_nr = visual_page_queue[visual_page_queue_head];
    visual_page_queue_head = (visual_page_queue_head+1)%MAX_VISUAL_PAGE_NUM;
    unsigned int va = PAGE2ADDR(va_nr);
    // copy original page to swap
	swap_rw(va_nr, va, ATA_WRITE); // write visual memory to swap area
    set_bit(swap_map, va_nr);
    unsigned int pa = PAGE2PAGE_TABLE(va_nr) & 0xFFFFF000;

    PAGE2PAGE_TABLE(nr) = pa | PG_USU | PG_RWW | PG_P;
    PAGE2PAGE_TABLE(va_nr) = 0;

    return;

}

PUBLIC void swap_rw(unsigned int page_nr, void* page_addr, int cmd_num) {
    printl("{PG} swap_rw: page_nr: %d, page_addr: %x, cmd_num: %d\n", page_nr, page_addr, cmd_num);
    void * la = page_addr;
    struct hd_cmd cmd;
    unsigned int byte_num = PAGE_SIZE;
    cmd.features = 0;
    cmd.count	= PAGE_SIZE/SECTOR_SIZE;
    cmd.lba_low	= page_nr & 0xFF;
    cmd.lba_mid	= (page_nr >>  8) & 0xFF;
    cmd.lba_high	= (page_nr >> 16) & 0xFF;
    cmd.device	= MAKE_DEVICE_REG(1, 2, (page_nr >> 24) & 0xF);
    cmd.command	= cmd_num;
    hd_cmd_out(&cmd);

    while (byte_num) {
        int bytes = min(SECTOR_SIZE, byte_num);
        if (cmd_num == ATA_READ) {

            MESSAGE msg;
            send_recv(RECEIVE, INTERRUPT, &msg);

            port_read(REG_DATA, la, SECTOR_SIZE);
        }
        else {
            if (!waitfor(STATUS_DRQ, STATUS_DRQ, HD_TIMEOUT))
                panic("hd writing error.");

            port_write(REG_DATA, la, bytes);

            MESSAGE msg;
            send_recv(RECEIVE, INTERRUPT, &msg);
        }
        byte_num -= SECTOR_SIZE;
        la += SECTOR_SIZE;
    }
}


