/* asbestOS -- a best OS*/

#ifndef KB_H
#define KB_H

#include "paging.h"
#include "lib.h"
#include "i8259.h"
#include "execute.h"


#define FALSE 0
#define TRUE 1

#define kb_irq_num 1
#define kb_in_port 0x60
#define kb_key_release 0x80

#define kb_scancode_size 128
#define kb_buf_size 128

#define ctrl 0x1d
#define alt 0x38
#define l_shift 0x2a
#define r_shift 0x36
#define caps_lock 0x3a
#define backspace 0x0e
#define enter 0x1c
#define f1 0x3B
#define f2 0x3C
#define f3 0x3D

#define x_size 80
#define x_max (x_size - 1)
#define y_size 25
#define y_max (y_size - 1)

#define VIDEO       0xB8000
#define ATTRIB      0x7


/* Local functions */
void keyboard_init();
void keyboard_handler();
void scroll_screen();
void scroll_screen_backgnd();
void term_putc(uint8_t c);
void term_putc_backgnd(uint8_t c);
void term_clear();

/* System call functions */
int32_t term_read(int32_t fd, void * buf, int32_t bytes);
int32_t term_write(int32_t fd, const void * buf, int32_t bytes);
int32_t term_open(const uint8_t* filename);
int32_t term_close(int32_t fd);

#endif
