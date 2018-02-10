/* asbestOS -- a best OS*/

#ifndef IDT_H
#define IDT_H

#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "rtc.h"
#include "idt_asm.h"
#include "execute.h"

#define RTC_IND         0x28
#define KB_IND          33
#define PIT_IND         0x20
#define VEC             256
#define SYS_EXCEPTIONS  31
#define SYS_CALL        0x80


void interrupt_0();
void interrupt_1();
void interrupt_2();
void interrupt_3();
void interrupt_4();
void interrupt_5();
void interrupt_6();
void interrupt_7();
void interrupt_8();
void interrupt_9();
void interrupt_10();
void interrupt_11();
void interrupt_12();
void interrupt_13();
void interrupt_14();
void interrupt_15();
void interrupt_16();
void interrupt_17();
void interrupt_18();
void interrupt_19();
void interrupt_kb();
void interrupt_intel();
void system_call();
void default_int();

void idt_init();


#endif
