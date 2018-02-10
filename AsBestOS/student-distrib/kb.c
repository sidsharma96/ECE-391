/* asbestOS -- a best OS*/

#include "kb.h"


static char shift_f = FALSE;
static char caps_lock_f = FALSE;
static char alt_f = FALSE;
static char ctrl_f = FALSE;
static char* video_mem = (char *)VIDEO;


int  x_arr[3] = {0,0,0};
int  y_arr[3] = {0,0,0};
int  x_prompt_arr[3] = {0,0,0};
int  y_prompt_arr[3] = {0,0,0};
int  pos_check = 0;

unsigned char kb_line_buf0[kb_buf_size];
unsigned char kb_line_buf1[kb_buf_size];
unsigned char kb_line_buf2[kb_buf_size];
unsigned char* kb_line_buf = kb_line_buf0;

unsigned int  kb_line_buf_pos0 = 0;
unsigned int  kb_line_buf_pos1 = 0;
unsigned int  kb_line_buf_pos2 = 0;
unsigned int* kb_line_buf_pos = &kb_line_buf_pos0;

// Track last typed key in each terminal
uint8_t out[3] = {'\0', '\0', '\0'};

// Pointers to 4KB pages we save offscreen terminals in
static unsigned int vid_pages[3] = {VIDEO_0, VIDEO_1, VIDEO_2};

// Terminal displayed on the screen
int open_terminal = 0;


static unsigned char scancode[3];


/* 
    This is the standard US Keyboard mapping function, 
    influence from http://www.osdever.net/bkerndev/Docs/keyboard.htm. 
*/
unsigned char kbdus[kb_scancode_size] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
    '9', '0', '-', '=', '\b',	/* Backspace */
    '\t',			/* Tab */
    'q', 'w', 'e', 'r',	/* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
    '\'', '`',   0,		/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
    'm', ',', '.', '/',   0,				/* Right shift */
    '*',
    0,	/* Alt */
    ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};


unsigned char kbdus_shift[kb_scancode_size] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
    '(', ')', '_', '+', '\b',   /* Backspace */
    '\t',           /* Tab */
    'Q', 'W', 'E', 'R', /* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',   /* Enter key */
    0,          /* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',   /* 39 */
    '\'', '~',   0,     /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',         /* 49 */
    'M', '<', '>', '?',   0,                /* Right shift */
    '*',
    0,  /* Alt */
    ' ',    /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};


unsigned char kbdus_caps[kb_scancode_size] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b',   /* Backspace */
    '\t',           /* Tab */
    'Q', 'W', 'E', 'R', /* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',   /* Enter key */
    0,          /* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',   /* 39 */
    '\'', '`',   0,     /* Left shift */
    '\\', 'Z', 'X', 'C', 'V', 'B', 'N',         /* 49 */
    'M', ',', '.', '/',   0,                /* Right shift */
    '*',
    0,  /* Alt */
    ' ',    /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};


unsigned char kbdus_shift_caps[kb_scancode_size] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
    '(', ')', '_', '+', '\b',   /* Backspace */
    '\t',           /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',   /* Enter key */
    0,          /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',   /* 39 */
    '\'', '~',   0,     /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',         /* 49 */
    'm', '<', '>', '?',   0,                /* Right shift */
    '*',
    0,  /* Alt */
    ' ',    /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};


/*
    void keyboard_init()
    This function initializes the keyboard, by enabling the IRQ at the PIC
    Inputs: None
    Outputs: Enables IRQ #1
    Return Value: none
*/
void keyboard_init()
{
	enable_irq(kb_irq_num);
}


/*
    void keyboard_handler()
    This function handles the keyboard interrupt, by reading in from the keyboard
    when the interrupt is called, printing whatever character was typed, and then 
    sending the EOI to the PIC. Influenced by http://www.osdever.net/bkerndev/Docs/keyboard.htm. 
    Inputs: none
    Outputs: keypress to the screen
    Return Value: none
*/
void keyboard_handler()
{
    scancode[open_terminal] = inb(kb_in_port);

    // This block of code sets the flags for all of the modifier keys which are currently used.
    if(scancode[open_terminal] == ctrl)
    {
        ctrl_f = TRUE;
    }
    else if(scancode[open_terminal] == (ctrl | kb_key_release))
    {
        ctrl_f = FALSE;
    }
    else if((scancode[open_terminal] == l_shift) | (scancode[open_terminal] == r_shift))
    {
        shift_f = TRUE;
    }
    else if((scancode[open_terminal] == (l_shift | kb_key_release)) | (scancode[open_terminal] == (r_shift | kb_key_release)))
    {
        shift_f = FALSE;
    }
    else if(scancode[open_terminal] == alt)
    {
        alt_f = TRUE;
    }
    else if(scancode[open_terminal] == (alt | kb_key_release))
    {
        alt_f = FALSE;
    }

    if((scancode[open_terminal] == caps_lock) && (caps_lock_f == FALSE))
    {
        caps_lock_f = TRUE;
    }
    else if((scancode[open_terminal] == caps_lock) && (caps_lock_f == TRUE))
    {
        caps_lock_f = FALSE;
    }

    // The character received is set from the array corresponding to which modifiers were present.
    if((shift_f == TRUE) && (caps_lock_f == FALSE))
    {
        out[open_terminal] = kbdus_shift[scancode[open_terminal]];
    }
    else if((shift_f == FALSE) && (caps_lock_f == TRUE))
    {
        out[open_terminal] = kbdus_caps[scancode[open_terminal]];
    }
    else if((shift_f == TRUE) && (caps_lock_f == TRUE))
    {
        out[open_terminal] = kbdus_shift_caps[scancode[open_terminal]];
    }
    else
    {
        out[open_terminal] = kbdus[scancode[open_terminal]];
    }

    // Print any standard characters by adding them to the line buffer.
    if((out[open_terminal] != 0) && (out[open_terminal] != '\b') && !(scancode[open_terminal] & kb_key_release))
    {
        if(*kb_line_buf_pos != (kb_buf_size - 1))
        {
            kb_line_buf[*kb_line_buf_pos] = out[open_terminal];
        }

        if(*kb_line_buf_pos < (kb_buf_size - 1)) 
        {
            *kb_line_buf_pos += 1;
            term_putc(out[open_terminal]);       
        }

        // Echo the character to the screen.
        // term_putc(out[open_terminal]);       
    }

    // If the key that was inputted was a backspace, remove that character 
    // from the line buffer and decrement the position.
    if((x_prompt_arr[open_terminal] == x_arr[open_terminal]) && (y_prompt_arr[open_terminal] == y_arr[open_terminal]))
    {
        pos_check = 1;
    }
    else
    {
        pos_check = 0;
    }
    
    if((out[open_terminal] == '\b') && !(scancode[open_terminal] & kb_key_release) && (pos_check == 0))
    {
        if(*kb_line_buf_pos > 0)
        {
            kb_line_buf[*kb_line_buf_pos] = ' ';
            *kb_line_buf_pos -= 1;
        }
        else if(*kb_line_buf_pos == 0)
        {
            kb_line_buf[*kb_line_buf_pos] = ' ';
        }
        
        // If the x_pos becomes negative, it must wrap back around to the previous line.
        x_arr[open_terminal]--;
        
        if((x_arr[open_terminal] < 0) && (y_arr[open_terminal] != 0))
        {
            x_arr[open_terminal] = x_max;
            y_arr[open_terminal]--;
        }
        else if((x_arr[open_terminal] < 0) && (y_arr[open_terminal] == 0))
        {
            x_arr[open_terminal] = 0;
            y_arr[open_terminal] = 0;
        }

        term_putc(' ');

        // Must repeat the above process, as term_putc increments the position counters.
        x_arr[open_terminal]--;
        if((x_arr[open_terminal] < 0) && (y_arr[open_terminal] != 0))
        {
            x_arr[open_terminal] = x_max;
            y_arr[open_terminal]--;
        }
        else if((x_arr[open_terminal] < 0) && (y_arr[open_terminal] == 0))
        {
            x_arr[open_terminal] = 0;
            y_arr[open_terminal] = 0;
        }   
    }

    // Clear screen if Ctrl + L was pressed
    if((ctrl_f == TRUE) && (out[open_terminal] == 'l'))
    {
        scancode[open_terminal] = (enter | kb_key_release);
        out[open_terminal] = '\n';
        *kb_line_buf_pos -= 1;
        kb_line_buf[*kb_line_buf_pos] = '\n';
        term_clear();
    }

    // Check for if a terminal switch is needed
    if(alt_f == TRUE)
    {
        if(scancode[open_terminal] == f1 || scancode[open_terminal] == f2 || scancode[open_terminal] == f3)
        {
            switch (scancode[open_terminal])
            {
                case f1:
                    if(open_terminal == 0)
                    {
                        break;
                    }

                    kb_line_buf = kb_line_buf0;
                    kb_line_buf_pos = &kb_line_buf_pos0;
                    out[1] = '\0';
                    out[2] = '\0';
                    send_eoi(kb_irq_num);
                    switch_vid_page(0);
                    break;
                
                case f2:
                    if(open_terminal == 1)
                    {
                        break;
                    }

                    kb_line_buf = kb_line_buf1;
                    kb_line_buf_pos = &kb_line_buf_pos1;
                    out[0] = '\0';
                    out[2] = '\0';
                    send_eoi(kb_irq_num);                  
                    switch_vid_page(1);

                    if(terminal_pcb_ptrs[1] == NULL)
                    {
                        change_running_process(1);
                    }

                    break;
                
                case f3:
                    if(open_terminal == 2)
                    {
                        break;
                    }
                    kb_line_buf = kb_line_buf2;
                    kb_line_buf_pos = &kb_line_buf_pos2;
                    out[0] = '\0';
                    out[1] = '\0';
                    send_eoi(kb_irq_num);
                    switch_vid_page(2);

                    if(terminal_pcb_ptrs[2] == NULL)
                    {
                        change_running_process(2);
                    }

                    break;
                default:
                    break;
            }            
        }
    }

    send_eoi(kb_irq_num);
}


/*
* term_open
*   Description:    This function is in place to preserve the system call primitive.
*                   In reality it only resets the position of the next character.
*   Inputs:     None
*   Outputs:    0 for success
*   Side effects: Resets position
*/
int32_t term_open(const uint8_t* filename)
{
    int j;
    for(j = 0; j < kb_buf_size; j++)
    {
        kb_line_buf[j] = '\0';
    }
    *kb_line_buf_pos = 0;
    x_arr[open_terminal] = 0;
    y_arr[open_terminal] = 0;
    return 0;
}


/*
* term_close
*   Description:    This function is in place to preserve the system call primitive.
*                   In reality it simply returns.
*   Inputs:     None
*   Outputs:    0 for success
*   Side effects: None
*/
int32_t term_close(int32_t fd)
{
    // This function essentially does nothing.
    return -1;
}


/*
* term_read
*   Description:    This function is the equivalent of scanf in the C library. It accepts a pointer 
*                   to a buffer and fills it with input received from the keyboard.
*   Inputs:     Pointer to a buffer to be filled, the number of bytes to receive.
*   Outputs:    The number of bytes read.
*   Side effects: Clears and refills the keyboard line buffer.
*/
int32_t term_read(int32_t fd, void * buf, int32_t bytes)
{
    if(buf == NULL)
    {
        return -1;
    }

    sti();
    int i, j;

    while(terminal_proc_currently_running != open_terminal);

    int local_open = open_terminal;
    
    while(out[local_open] != '\n');
    
    // Do nothing until end of input.
    while(scancode[local_open] != (enter | kb_key_release));
    
    out[local_open] = '\0';

    switch(local_open){
        case 0:
            kb_line_buf = kb_line_buf0;
            kb_line_buf_pos = &kb_line_buf_pos0;
            break;
        
        case 1:
            kb_line_buf = kb_line_buf1;
            kb_line_buf_pos = &kb_line_buf_pos1;
            break;
        
        case 2:
            kb_line_buf = kb_line_buf2;
            kb_line_buf_pos = &kb_line_buf_pos2;
            break;
    }

    for(i = 0; (i < (bytes - 1)) && (i < (kb_buf_size - 1)); i++)
    {
        if(kb_line_buf[i] == '\n')
        {
            ((char*)buf)[i] = kb_line_buf[i];
            
            for(j = 0; j < kb_buf_size; j++)
            {
                    kb_line_buf[j] = '\0';
            }
            
            *kb_line_buf_pos = 0;
            
            return (i + 1);
        }
        else
        {
            ((char*)buf)[i] = kb_line_buf[i];
        }
    }

    // If the buffer has been filled, manually make the last line the line feed character.
    if(bytes < kb_buf_size)
    {
        i = bytes - 1;
    }    
    else
    {
        i = kb_buf_size - 1;
    }

    ((char*)buf)[i] = '\n';

    for(j = 0; j < kb_buf_size; j++)
    {
        kb_line_buf[j] = '\0';
    }

    *kb_line_buf_pos = 0;
    cli();

    return i;
}


/*
* term_write
*   Description:    This function is the equivalent of printf in the C library. It accepts a pointer 
*                   to a buffer and prints it to the terminal.
*   Inputs:     Pointer to a buffer to print, the number of bytes to print.
*   Outputs:    0 for success.
*   Side effects: Changes x_pos and y_pos by virtue of term_putc.
*/
int32_t term_write(int32_t fd, const void * buf, int32_t bytes){
    int i;

    if(buf == NULL)
    {
        return -1;
    }

    for(i = 0; i < bytes; i++)
    {
        terminal_proc_currently_running = current_pcb_ptr->term_id;
        if(terminal_proc_currently_running != open_terminal)
        {
            term_putc_backgnd(((char*)buf)[i]);
        }
        else
        {
            term_putc(((char*)buf)[i]);    
        }
    }

    if(terminal_proc_currently_running == open_terminal)
    {
        x_prompt_arr[open_terminal] = x_arr[open_terminal];
        y_prompt_arr[open_terminal] = y_arr[open_terminal];
    }
    else
    {
        x_prompt_arr[terminal_proc_currently_running] = x_arr[terminal_proc_currently_running];
        y_prompt_arr[terminal_proc_currently_running] = y_arr[terminal_proc_currently_running];
    }

    return 0;
}


/*
*  term_putc
*   Description:    This function takes a character and places it at the desired
*                   location in video memory. In the case of the enter character,
*                   a new line is created. In the case of the screen being filled,
*                   the screen is scrolled upwards.
*   Inputs: c, the character to be printed
*   Outputs: none
*   Side effects: interrupts are briefly blocked while writing to video memory.
*/
void term_putc(uint8_t c){
    // Handle the case of an enter character and possible required scrolling
    int local_open = open_terminal;

    if(c == '\n' || c == '\r')
    {
        x_arr[local_open] = 0;
        y_arr[local_open]++;

        if(y_arr[local_open] > y_max)
        {
            scroll_screen();
            y_arr[local_open] = y_max;
        }
    }

    // Handle regular characters
    else
    {
        cli();
        *(uint8_t *)(video_mem + ((x_size * y_arr[local_open] + x_arr[local_open]) << 1)) = c;
        *(uint8_t *)(video_mem + ((x_size * y_arr[local_open] + x_arr[local_open]) << 1) + 1) = ATTRIB;
        sti();
        // Wrap text if the current row has been filled and scroll screen if this is the bottom row.
        x_arr[local_open]++;
        if((x_arr[local_open] / x_size) >= 1)
        {
            x_arr[local_open] = 0;
            y_arr[local_open]++;
            if(y_arr[local_open] > y_max)
            {
                scroll_screen();
                y_arr[local_open] = y_max;
            }
        }
    }
}

/*
*  term_putc_backnd
*   Description:    This function takes a character and places it at the desired
*                   location in video memory of a background terminal. 
*   Inputs: c, the character to be printed
*   Outputs: none
*   Side effects: interrupts are briefly blocked while writing to video memory.
*/
void term_putc_backgnd(uint8_t c){
    // Handle the case of an enter character and possible required scrolling
    int local_open = terminal_proc_currently_running;

    if(c == '\n' || c == '\r')
    {
        x_arr[local_open] = 0;
        y_arr[local_open]++;
        if(y_arr[local_open] > y_max)
        {
            scroll_screen_backgnd();
            y_arr[local_open] = y_max;
        }
    }
    else
    {
        // Handle regular characters

        cli();
        *(uint8_t *)((char*) vid_pages[local_open] + ((x_size * y_arr[local_open] + x_arr[local_open]) << 1)) = c;
        *(uint8_t *)((char*) vid_pages[local_open] + ((x_size * y_arr[local_open] + x_arr[local_open]) << 1) + 1) = ATTRIB;
        sti();

        //  Wrap text if the current row has been filled and scroll screen if this is the bottom row.
        x_arr[local_open]++;
        if((x_arr[local_open] / x_size) >= 1)
        {
            x_arr[local_open] = 0;
            y_arr[local_open]++;

            if(y_arr[local_open] > y_max)
            {
                scroll_screen_backgnd();
                y_arr[local_open] = y_max;
            }
        }
    }
}


/*
*  scroll_screen
*   Description: This function shifts all rows of the display up, as well as clearing the
*                bottom row in order that it be ready for new text to be displayed.  This
*                is achieved by offsetting all elements of video memory by the width of the display.
*   Inputs: None
*   Outputs: None
*   Side effects: interrupts are briefly blocked while writing to video memory.
*/
void scroll_screen()
{
    int i, j;

    cli();

    //  First, shift rows 1-79 up by one y index
    for(j = 0; j < y_max; j++)
    {
        for(i = 0; i < x_size; i ++)
        {
            *(uint8_t *)(video_mem + ((x_size * j + i) << 1)) = *(uint8_t *)(video_mem + ((x_size * (j + 1) + i) << 1));
            *(uint8_t *)(video_mem + ((x_size * j + i) << 1) + 1) =  ATTRIB;
        }
    }

    //  Next, clear the bottom row of entries
    for(i = 0; i < x_size; i ++)
    {
        *(uint8_t *)(video_mem + ((x_size * y_max + i) << 1)) = ' ';
        *(uint8_t *)(video_mem + ((x_size * y_max + i) << 1) + 1) = ATTRIB;
    }

    sti();
}


/*
*  scroll_screen_backgnd
*   Description: This function shifts all rows of the display up of a background terminal, as well as clearing the
*                bottom row in order that it be ready for new text to be displayed.  This
*                is achieved by offsetting all elements of video memory by the width of the display.
*   Inputs: None
*   Outputs: None
*   Side effects: interrupts are briefly blocked while writing to video memory.
*/
void scroll_screen_backgnd()
{
    int i, j;

    cli();

    //  First, shift rows 1-79 up by one y index
    for(j = 0; j < y_max; j++)
    {
        for(i = 0; i < x_size; i ++)
        {
            *(uint8_t *)((char*) vid_pages[terminal_proc_currently_running] + ((x_size * j + i) << 1)) = *(uint8_t *)((char*) vid_pages[terminal_proc_currently_running] + ((x_size * (j + 1) + i) << 1));
            *(uint8_t *)((char*) vid_pages[terminal_proc_currently_running] + ((x_size * j + i) << 1) + 1) =  ATTRIB;
        }
    }

    //  Next, clear the bottom row of entries
    for(i = 0; i < x_size; i ++)
    {
        *(uint8_t *)((char*) vid_pages[terminal_proc_currently_running] + ((x_size * y_max + i) << 1)) = ' ';
        *(uint8_t *)((char*) vid_pages[terminal_proc_currently_running] + ((x_size * y_max + i) << 1) + 1) = ATTRIB;
    }

    sti();
}


/*
*  term_clear
*   Description:   This function clears the currently displayed video memory
*   Inputs: None
*   Outputs: none
*/
void term_clear(){
    clear();
    x_arr[open_terminal] = 0;
    y_arr[open_terminal] = 0;
}
