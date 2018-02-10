/* tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) \
	printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)


static unsigned char seg[16] = {0xE7, 0x06, 0xCB, 0x8F, 0x2E, 0xAD, 0x6D, 0x86, 0xEF, 0xAE, 0xEE, 0xEF, 0xE1, 0xE7, 0xE9, 0xE8};
static unsigned int ackno_ = 0;
static unsigned long but_init = 0x0;
static unsigned long led_init;
static spinlock_t red_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t button_lock = SPIN_LOCK_UNLOCKED;
static unsigned int packetflag = 0;



int tux_set_led(struct tty_struct* tty, unsigned long info);
void changestate(unsigned char let, unsigned char dir);
int tux_buttons(struct tty_struct* tty, unsigned long info);
int tux_init(struct tty_struct* tty);
void reset(struct tty_struct* tty);
/************************ Protocol Implementation *************************/

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in 
 * tuxctl-ld.c. It calls this function, so all warnings there apply 
 * here as well.
 */

void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet)
{
	unsigned a, b, c;
    
	spin_lock(&red_lock);

	if(packetflag){
		return;
	}

    

    a = packet[0]; /* Avoid printk() sign extending the 8-bit */
    b = packet[1]; /* values when printing them. */
    c = packet[2];

    switch(a){

    	case MTCP_ACK:
    		ackno_ = 1;
    		break;

    	case MTCP_BIOC_EVENT:
    		changestate(b,c);
    		break;
    	
    	case MTCP_RESET:
    		reset(tty);
    		break;
    }

    spin_unlock(&red_lock);

    return;
    /*printk("packet : %x %x %x\n", a, b, c); */

}
/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/
int 
tuxctl_ioctl (struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg)
{
	int a;
    switch (cmd) {
	case TUX_INIT:
		a = tux_init(tty);
		break;
	case TUX_BUTTONS:
		a = tux_buttons(tty, arg);
		break;
	case TUX_SET_LED:
		a = tux_set_led(tty, arg);
		break;
	case TUX_LED_ACK:
		a = 0;
		break;
	case TUX_LED_REQUEST:
		a = 0;
		break;
	case TUX_READ_LED:
		a = 0;
		break;
	default:
	    a = -EINVAL;
    }
    return a;
}


int tux_init(struct tty_struct* tty){

	char buf[2];

	printk("You hit init\n");

	buf[0] = MTCP_BIOC_ON;
	buf[1] = MTCP_LED_USR;
	tuxctl_ldisc_put(tty, buf, 2);

	led_init = 0;
	ackno_ = 0;
	but_init = 0x0;
	button_lock = SPIN_LOCK_UNLOCKED;

	printk("Exit init\n");

	return 0;
}



int tux_set_led(struct tty_struct* tty, unsigned long info){

	unsigned leddis, decimal, hexdis;
	char buffer[7];
	int i;


	if(!ackno_){
		return -1;
	}

	ackno_ = 0;

	//printk("LED1\n");
	hexdis = info & 0x0000FFFF;

	leddis = info & 0x000F0000;
	leddis = leddis >> 16;

	decimal = info & 0x0F000000;
	decimal = decimal >> 24;

	buffer[0] = MTCP_LED_USR;
	buffer[1] = MTCP_LED_SET;
	buffer[2] = 0xF;
	//printk("for loop\n");
	for (i=0; i<4; i++){

		if((leddis>>i)&0x1){
			buffer[3+i] = seg[(0x000000F&(hexdis >> 4*i))];
			if((decimal>>i)&0x1){
				buffer[3+i] = buffer[3+i] | 0x10;

			}

			//printk("buf[%d]: %x\n", 2+i, buffer[2+i]);
			
		}

		else{
			buffer[3+i]= 0x0;
 			//printk("34");
		}
	}

	led_init = info;
	//printk("put\n");


	tuxctl_ldisc_put( tty,buffer, 7);

	return 0;
}


void changestate(unsigned char let, unsigned char dir){
	unsigned long flags;
	int left =0;
	int down =0;
	dir = ~(dir);
	let = ~(let);


	let = let & 0x0F;
	dir = dir & 0x0F;


	//printk("State1 %x \n", but_init);
	
	if((dir>>1) & 0x01){
		left = 1;
	}

	
	if((dir>>2) & 0x01){
		down = 1;
	}
	

	dir = (dir & 0x09);

	//printk("State2 %x \n", but_init);

	if(left){
		dir = dir | 0x04;
	}

	if(down){
		dir = dir | 0x02;
	}

	//printk("State3 %x \n", but_init);

	dir = dir<<4;

	//printk("State4 %x \n", but_init);
	dir = dir + let;

	spin_lock_irqsave(&button_lock, flags);
	but_init = dir;
	//printk("State5 %x \n", but_init);
	spin_unlock_irqrestore(&button_lock, flags);

	


}


int tux_buttons(struct tty_struct* tty, unsigned long info){

	//printk("Entering button");
	unsigned long flags;
	int c;
	
	spin_lock_irqsave(&button_lock, flags);
	c = copy_to_user((void*) info, (void*) &but_init, sizeof(long));
	spin_unlock_irqrestore(&button_lock, flags);
	if (c!=0){
		return -EINVAL;
	}
	//printk("Leaving button");

	return 0;

}


void reset(struct tty_struct* tty){
	char buf[2];

	unsigned long flags;

	ackno_ = 0;

	buf[0] = MTCP_BIOC_ON;
	buf[1] = MTCP_LED_USR;

	tuxctl_ldisc_put(tty, buf, 2);

	led_init = 0;
	button_lock = SPIN_LOCK_UNLOCKED;
	
	spin_lock_irqsave(&button_lock, flags);
	but_init = 0x0;
	spin_unlock_irqrestore(&button_lock, flags);


	tux_set_led(tty, led_init);

}



