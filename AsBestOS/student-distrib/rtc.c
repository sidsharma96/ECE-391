/* asbestOS -- a best OS*/

#include "rtc.h"
#include "kb.h"
#include "filesystem.h"
#include "execute.h"


/*
Global variables
*/
static volatile int rtc_flag[3] = {0, 0, 0};
static general_ops_table_t rtc_ops_table_desc = {rtc_open, rtc_close, 
                                                    rtc_read, rtc_write};


/* void rtc_init()
 * Inputs: none
 * Outputs: Enables IRQ #8
 * Return Value: none
 * Function: Initializes the RTC with the required register B and ports 0x70 and 0x71. Enables IRQ#8, which is IRQ#1 on the slave PIC.
 * Referenced from http://wiki.osdev.org/RTC
 */
void rtc_init()
{
	char prev;

	outb(REGB, P1);				// selecting register B and disabling NMI
	prev = inb(P2);				// reading register B
	outb(REGB, P1);				// selecting register B again
	outb(prev | BITMASK, P2);	// Turning on bit 6 of register B

	enable_irq(rtc_irq_number); // Enabling IRQ#8
}


/* void rtc_handler()
 * Inputs: none
 * Outputs: Enables IRQ #8
 * Return Value: none
 * Function: Handles the RTC interrupt, throws away the contents, and then sends an EOI to the PIC.
 * Refrenced from http://wiki.osdev.org/RTC
 */
void rtc_handler()
{
	unsigned long flags;
	cli_and_save(flags);
	rtc_flag[0] = 1;
	rtc_flag[1] = 1;
	rtc_flag[2] = 1;

	// Select register C
	outb(REGC, P1);

	// Throw away contents
	inb(P2);				
	restore_flags(flags);

	// Send EOI to PIC
	send_eoi(rtc_irq_number); 
}


/* int32_t rtc_open (const uint8_t* filename)
 * Inputs: pointer to filename
 * Outputs: none
 * Return Value: returns the file descriptor
 * Function: Initializes the rtc and sets its frequency to 2 Hz
 * Refrenced from http://wiki.osdev.org/RTC
 */
int32_t rtc_open (const uint8_t* filename)
{
	int retVal;
	int32_t buf[1];
	buf[0] = 2;
	rtc_write(0, buf, 4);

    retVal = find_open_desc_index();
    if(retVal == -1)
    {
        return retVal;
    }

    current_pcb_ptr->open_files[retVal].file_ops_table_ptr = &rtc_ops_table_desc;
    current_pcb_ptr->open_files[retVal].inode = 0;
    current_pcb_ptr->open_files[retVal].file_pos = 0;
    current_pcb_ptr->open_files[retVal].flags = 1;

    return retVal;
}


/* int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes)
 * Inputs: file descriptor, buffer, and number of bytes to be read
 * Outputs: None
 * Return Value: Returns 0
 * Function: This function should always return 0, but only after an interrupt has ocurred (sets a flag and waits until the interrupt handler clears it, then returns 0).
 */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes)
{	
	sti();
	while(!rtc_flag[terminal_proc_currently_running]);
	rtc_flag[terminal_proc_currently_running] = 0;
	cli();

	return 0;
}


/* int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes)
 * Inputs: file descriptor, buffer, and number of bytes to be written
 * Outputs: None
 * Return Value: -1 on failure, number of bytes to be written on success
 * Function: Sets the frequency at which the RTC is set at. This makes the handler occur only after a defined period of time.
 * Refrenced from http://wiki.osdev.org/RTC
 */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes)
{
	if (nbytes != 4 || buf == NULL)
	{
		return -1;
	}
	
	int32_t fre;
	
	char rate;
	char prev;

	fre = *((int32_t*) buf);

	switch(fre){
		case(2):
			rate = 0x0F;
			break;
		case(4):
			rate = 0x0E;
			break;
		case(8):
			rate = 0x0D;
			break;
		case(16):
			rate = 0x0C;
			break;
		case(32):
			rate = 0x0B;
			break;
		case(64):
			rate = 0x0A;
			break;
		case(128):
			rate = 0x09;
			break;
		case(256):
			rate = 0x08;
			break;
		case(512):
			rate = 0x07;
			break;
		case(1024):
			rate = 0x06;
			break;
		default:
			return -1;

	}


	outb(REGA, P1);				// selecting register A and disabling NMI
	prev = inb(P2);				// reading register A
	outb(REGA, P1);				// selecting register A again
	outb( (prev&0xF0) | rate, P2);	// Turning on bit 4 of register A

	return nbytes;
}


/* int32_t rtc_close (int32_t fd)
 * Inputs: int file descriptor
 * Outputs: Nothing
 * Return Value: Returns 0
 * Function: Just closes the rtc interrupt
 */
int32_t rtc_close (int32_t fd)
{
    if(fd > 0 && fd < FD_TABLE_SIZE && current_pcb_ptr->open_files[fd].flags)
    {
        current_pcb_ptr->open_files[fd].flags = 0;
        return 0;
    }
    return -1;
}

