/* asbestOS -- a best OS*/

#include "idt.h"
#include "kb.h"


/* void interrupt_0()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Divide by Zero error
 */
void interrupt_0()
{
	cli();
	printf("Divide Error \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_1()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Reserved error
 */
void interrupt_1()
{
	cli();
	printf("Reserved \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_2()
 * Inputs: none
 * Return Value: none
 * Function: Handles the NMI interrupt error
 */
void interrupt_2()
{
	cli();
	printf("NMI interrupt \n");
	while(1);
}


/* void interrupt_3()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Breakpoint error
 */
void interrupt_3()
{
	cli();
	printf("Breakpoint \n");
	while(1);
}


/* void interrupt_4()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Overflow error
 */
void interrupt_4()
{
	cli();
	printf("Overflow \n");
	while(1);
}


/* void interrupt_5()
 * Inputs: none
 * Return Value: none
 * Function: Handles the BOUND Range Exceeded error
 */
void interrupt_5()
{
	cli();
	printf("BOUND Range Exceeded \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_6()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Invalid Opcode error
 */
void interrupt_6()
{
	cli();
	printf("Invalid Opcode (Undefined Opcode) \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_7()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Device Not Available error
 */
void interrupt_7()
{
	cli();
	printf("Device Not Available (No Math Coprocessor) \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_8()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Double Fault error
 */
void interrupt_8()
{
	printf("Double Fault \n");
	while(1);
}


/* void interrupt_9()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Coprocessor Segment Overrun error
 */
void interrupt_9()
{
	cli();
	printf("Coprocessor Segment Overrun (reserved) \n");
	while(1);
}


/* void interrupt_10()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Invalid TSS error
 */
void interrupt_10()
{
	cli();
	printf("Invalid TSS \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_11()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Segment not present error
 */
void interrupt_11()
{
	cli();
	printf("Segment not present \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_12()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Stack-Segment Fault error
 */
void interrupt_12()
{
	cli();
	printf("Stack-Segment Fault \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_13()
 * Inputs: none
 * Return Value: none
 * Function: Handles the General Protection error
 */

void interrupt_13()
{
	cli();
	printf("General Protection \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_14()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Page Fault error
 */
void interrupt_14()
{
	cli();
	printf("Page Fault \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_15()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Intel Reserved errors
 */
void interrupt_15()
{
	cli();
	printf("Intel Reserved\n");
	while(1);
}


/* void interrupt_16()
 * Inputs: none
 * Return Value: none
 * Function: Handles the FPU Floating-Point Error error
 */
void interrupt_16()
{
	cli();
	printf("x87 FPU Floating-Point Error (Math Fault) \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_17()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Alignment Check error
 */
void interrupt_17()
{
	cli();
	printf("Alignment Check \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_18()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Machine Check error
 */
void interrupt_18()
{
	cli();
	printf("Machine Check \n");
	while(1);
}


/* void interrupt_19()
 * Inputs: none
 * Return Value: none
 * Function: Handles the SIMD Floating-Point Exception
 */
void interrupt_19()
{
	cli();
	printf("SIMD Floating-Point Exception \n");
	sti();
	halt_exception();
	while(1);
}


/* void interrupt_intel()
 * Inputs: none
 * Return Value: none
 * Function: Handles the Intel exceptions
 */
void interrupt_intel()
{
	cli();
	printf("Intel Reserved \n");
	while(1);
}


/* void default_int()
 * Inputs: none
 * Return Value: none
 * Function: Handles the remaining interrupts
 */
void default_int()
{
	printf("default_int \n");
}


void system_call(){
	printf("Should not be called at this point.\n");
	while(1);
}


/* void idt_init()
 * Inputs: none
 * Return Value: none
 * Function: Initializes the IDT with the 20 exceptions, and the (256-20)=236 remaining software handlers
 */
void idt_init()
{
	int i;

	SET_IDT_ENTRY(idt[0], WRAPPER_0);
	SET_IDT_ENTRY(idt[1], WRAPPER_1);
	SET_IDT_ENTRY(idt[2], WRAPPER_2);
	SET_IDT_ENTRY(idt[3], WRAPPER_3);
	SET_IDT_ENTRY(idt[4], WRAPPER_4);
	SET_IDT_ENTRY(idt[5], WRAPPER_5);
	SET_IDT_ENTRY(idt[6], WRAPPER_6);
	SET_IDT_ENTRY(idt[7], WRAPPER_7);
	SET_IDT_ENTRY(idt[8], WRAPPER_8);
	SET_IDT_ENTRY(idt[9], WRAPPER_9);
	SET_IDT_ENTRY(idt[10], WRAPPER_10);
	SET_IDT_ENTRY(idt[11], WRAPPER_11);
	SET_IDT_ENTRY(idt[12], WRAPPER_12);
	SET_IDT_ENTRY(idt[13], WRAPPER_13);
	SET_IDT_ENTRY(idt[14], WRAPPER_14);
	SET_IDT_ENTRY(idt[15], WRAPPER_15);
	SET_IDT_ENTRY(idt[16], WRAPPER_16);
	SET_IDT_ENTRY(idt[17], WRAPPER_17);
	SET_IDT_ENTRY(idt[18], WRAPPER_18);
	SET_IDT_ENTRY(idt[19], WRAPPER_19);
	SET_IDT_ENTRY(idt[20], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[21], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[22], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[23], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[24], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[25], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[26], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[27], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[28], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[29], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[30], INTEL_WRAPPER);
	SET_IDT_ENTRY(idt[31], INTEL_WRAPPER);

	SET_IDT_ENTRY(idt[KB_IND], KB_WRAPPER); 		// IDT entry for keyboard
	SET_IDT_ENTRY(idt[RTC_IND], RTC_WRAPPER);	  	// IDT entry for rtc
	SET_IDT_ENTRY(idt[PIT_IND], PIT_WRAPPER);	  	// IDT entry for pit

	// Set all fields as required
	for (i =0; i <VEC; i++)
	{
		idt[i].present = 1;
		idt[i].dpl = 0;
		idt[i].reserved0 = 0;
		idt[i].size = 1;
		idt[i].reserved1 = 1;
		idt[i].reserved2 = 1;
		idt[i].reserved3 = 1;
		idt[i].reserved4 = 0;
		idt[i].seg_selector = KERNEL_CS;

		if(i == SYS_CALL)
		{
			SET_IDT_ENTRY(idt[i], SYS_CALL_WRAPPER);
			idt[i].dpl = 3;
		}
		else if(i > SYS_EXCEPTIONS && i != KB_IND && i != RTC_IND && i != PIT_IND)
		{
			// Set all other fields except keyboard, pit, and rtc
			SET_IDT_ENTRY(idt[i], default_int);
		}
	}

}
