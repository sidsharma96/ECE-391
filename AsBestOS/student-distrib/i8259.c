/* asbestOS -- a best OS*/

/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/*
    void i8259_init(void)
    Initialize the 8259 PIC
    Inputs: None
    Outputs: None
    Return Value: none
*/
void i8259_init(void)
{
    master_mask = 0xFF;//f9
    slave_mask = 0xFF;

    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);

    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);

    outb(ICW3_MASTER, MASTER_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);

    outb(ICW4, MASTER_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    outb(slave_mask, SLAVE_8259_DATA);
    outb(master_mask, MASTER_8259_DATA);

    enable_irq(slave_pic_irq_port);
}


/*
    void enable_irq(uint32_t irq_num)
    
    Enable (unmask) the specified IRQ
    Inputs: uint32_t irq_num, the IRQ number that is being enabled
    Outputs: Enables the passed in IRQ_NUM
    Return Value: none
*/
void enable_irq(uint32_t irq_num)
{
    if(irq_num > master_max_irq_port)
    {
        //slave, so mask on slave
        slave_mask &= ~(1 << (irq_num - 8));
        outb(slave_mask, SLAVE_8259_DATA);        
    }
    else
    {
        //mask on master
        master_mask &= ~(1 << (irq_num));
        outb(master_mask, MASTER_8259_DATA);
    }
}


/*
    void disable_irq(uint32_t irq_num)
    Disable (mask) the specified IRQ
    Inputs: uint32_t irq_num, the IRQ number that is being disabled
    Outputs: Disabled the passed in IRQ_NUM
    Return Value: none
*/
void disable_irq(uint32_t irq_num)
{
    if(irq_num > master_max_irq_port){
        //slave, so mask on slave
        slave_mask |= (1 << (irq_num - 8));
        outb(slave_mask, SLAVE_8259_PORT);
    }
    else
    {
        //mask on master
        master_mask |= (1 << (irq_num));
        outb(master_mask, MASTER_8259_PORT);
    }
}


/*
    void send_eoi(uint32_t irq_num)
    Send end-of-interrupt signal for the specified IRQ, influenced by http://wiki.osdev.org/8259_PIC#End_of_Interrupt
    Inputs: uint32_t irq_num, the IRQ number that is being disabled through the EOI
    Outputs: Sends the eoi for the passed in IRQ_NUM
    Return Value: none
*/
void send_eoi(uint32_t irq_num)
{
    if(irq_num > master_max_irq_port){
        outb(EOI|(irq_num-slave_min_irq), SLAVE_8259_PORT);
        outb(EOI|slave_pic_irq_port, MASTER_8259_PORT);
    }
    else
    {
        outb(EOI|irq_num, MASTER_8259_PORT);
    }
}
