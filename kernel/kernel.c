#ifndef _YOS_KERNEL_
#define _YOS_KERNEL_ 1

#include <type.h>
#include <x86.h>
#include <protect.h>
#include <string.h>
#include <interrupt.h>

#define GDT_SIZE  4
#define IDT_SIZE  256
#define DESC_SIZE 8
#define GATE_SIZE 8
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

#endif

Descriptor m_gdt[GDT_SIZE];
Gate m_idt[IDT_SIZE];
 
void setGDT();
void set_8259A();
void setIDT();
void getMenInfo();
void setPage();


void cmain(uint magic, uint addr){
	setGDT(); //先设置新GDT, printf需要用到GS
	cls(); //清屏
	/*
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf("magic in error!");
		return;
	}else printf("it is in kernel ! magic is %x\n", magic);
	
	set_8259A();
	setIDT();
	*/
}

void setGDT(){
	//init_descriptor(描述符地址, 段基址, 段限界, 属性)
	init_descriptor(&m_gdt[0], 0, 0, 0);	//空描述符
	init_descriptor(&m_gdt[1], 0, 0x0FFFFF, DA_CR|DA_32|DA_LIMIT_4K);	//cs 存在的可执行可读代码段
	init_descriptor(&m_gdt[2], 0, 0x0FFFFF, DA_DRW|DA_LIMIT_4K|DA_32);	//ds es ss 存在的可读写数据段
	init_descriptor(&m_gdt[3], BASE_ADDR_OF_GS, 0xFFFF, DA_DRW|DA_DPL3); //gs 基地址 0xB8000

	GdtPtr p = {
		.limit = (GDT_SIZE * DESC_SIZE - 1), 
		.addr_low = (uint)m_gdt, 
		.addr_high = (uint)m_gdt >> 16
	};

	_lgdt(&p);
}

//void _out_byte(ushort port, uchar data);
void set_8259A(){
	_out_byte(0x20, 0x11);
	_out_byte(0xA0, 0x11);
	_out_byte(0x21, 0x20);	//主8259A入口地址 20H
	_out_byte(0xA1, 0x28);	//主8259A入口地址 28H
	_out_byte(0x21, 0x04);
	_out_byte(0xA1, 0x02);
	_out_byte(0x21, 0x01);
	_out_byte(0xA1, 0x01);
	_out_byte(0x21, 0xFF);  //OCW1 0xFF=11111111B 屏蔽所有主8259中断
	_out_byte(0xA1, 0xFF);	//OCW1 0xFF=11111111B 屏蔽所有从8259中断
}

void setIDT(){
	//init_gate(门地址, 目标选择子, 偏移, DCount, 属性)
	init_gate(&m_idt[0],  SELECTOR_CODE, (uint)divide_error, 0, DA_386IGate);
	init_gate(&m_idt[1],  SELECTOR_CODE, (uint)single_step_exception, 0, DA_386IGate);
	init_gate(&m_idt[2],  SELECTOR_CODE, (uint)nmi, 0, DA_386IGate);
	init_gate(&m_idt[3],  SELECTOR_CODE, (uint)breakpoint_exception, 0, DA_386IGate);
	init_gate(&m_idt[4],  SELECTOR_CODE, (uint)overflow, 0, DA_386IGate);
	init_gate(&m_idt[5],  SELECTOR_CODE, (uint)bounds_check, 0, DA_386IGate);
	init_gate(&m_idt[6],  SELECTOR_CODE, (uint)inval_opcode, 0, DA_386IGate);
	init_gate(&m_idt[7],  SELECTOR_CODE, (uint)copr_not_available, 0, DA_386IGate);
	init_gate(&m_idt[8],  SELECTOR_CODE, (uint)double_fault, 0, DA_386IGate);
	init_gate(&m_idt[9],  SELECTOR_CODE, (uint)copr_seg_overrun, 0, DA_386IGate);
	init_gate(&m_idt[10], SELECTOR_CODE, (uint)inval_tss, 0, DA_386IGate);
	init_gate(&m_idt[11], SELECTOR_CODE, (uint)segment_not_present, 0, DA_386IGate);
	init_gate(&m_idt[12], SELECTOR_CODE, (uint)stack_exception, 0, DA_386IGate);
	init_gate(&m_idt[13], SELECTOR_CODE, (uint)general_protection, 0, DA_386IGate);
	init_gate(&m_idt[14], SELECTOR_CODE, (uint)page_fault, 0, DA_386IGate);
	//没有15
	init_gate(&m_idt[16], SELECTOR_CODE, (uint)copr_error, 0, DA_386IGate);

	init_gate(&m_idt[32 +  0], SELECTOR_CODE, (uint)IRQ_00, 0, DA_386IGate);
	init_gate(&m_idt[32 +  1], SELECTOR_CODE, (uint)IRQ_01, 0, DA_386IGate);
	init_gate(&m_idt[32 +  2], SELECTOR_CODE, (uint)IRQ_02, 0, DA_386IGate);
	init_gate(&m_idt[32 +  3], SELECTOR_CODE, (uint)IRQ_03, 0, DA_386IGate);
	init_gate(&m_idt[32 +  4], SELECTOR_CODE, (uint)IRQ_04, 0, DA_386IGate);
	init_gate(&m_idt[32 +  5], SELECTOR_CODE, (uint)IRQ_05, 0, DA_386IGate);
	init_gate(&m_idt[32 +  6], SELECTOR_CODE, (uint)IRQ_06, 0, DA_386IGate);
	init_gate(&m_idt[32 +  7], SELECTOR_CODE, (uint)IRQ_07, 0, DA_386IGate);
	init_gate(&m_idt[32 +  8], SELECTOR_CODE, (uint)IRQ_08, 0, DA_386IGate);
	init_gate(&m_idt[32 +  9], SELECTOR_CODE, (uint)IRQ_09, 0, DA_386IGate);
	init_gate(&m_idt[32 + 10], SELECTOR_CODE, (uint)IRQ_10, 0, DA_386IGate);
	init_gate(&m_idt[32 + 11], SELECTOR_CODE, (uint)IRQ_11, 0, DA_386IGate);
	init_gate(&m_idt[32 + 12], SELECTOR_CODE, (uint)IRQ_12, 0, DA_386IGate);
	init_gate(&m_idt[32 + 13], SELECTOR_CODE, (uint)IRQ_13, 0, DA_386IGate);
	init_gate(&m_idt[32 + 14], SELECTOR_CODE, (uint)IRQ_14, 0, DA_386IGate);
	init_gate(&m_idt[32 + 15], SELECTOR_CODE, (uint)IRQ_15, 0, DA_386IGate);

	IdtPtr p = {
		.limit = (GATE_SIZE * IDT_SIZE - 1),
		.addr_low = (uint)m_idt, 
		.addr_high = (uint)m_idt >> 16
	};

	_lidt(&p);
}