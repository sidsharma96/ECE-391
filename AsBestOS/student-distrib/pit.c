/* asbestOS -- a best OS*/

#include "pit.h"


/*
    void pit_init()
    This function initializes the pit
    Inputs: None
    Outputs: none, initialized pit
    Return Value: None
*/
void pit_init()
{
	outb(DATA1, PIT_P1);
	outb(((PIT_FREQ/VALUE_SET) & 0xFF), PIT_P2);
	outb(((PIT_FREQ/VALUE_SET) >> 8), PIT_P2);

    // Enable IRQ#0
	enable_irq(PIT_IRQ_NUM); 

}

/*
    void pit_handler()
    This function handles the pit interrupt by calling schedule_next Proc
    Inputs: None
    Outputs: none
    Return Value: None

*/
void pit_handler()
{
    // Send EOI to PIT
    send_eoi(PIT_IRQ_NUM);
    schedule_next_proc();
}
