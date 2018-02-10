/* asbestOS -- a best OS*/

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "kb.h"
#include "lib.h"
#include "execute.h"

#define NUM_ENTRIES 1024
#define TABLE_SIZE 4096

#define VIDEO 	0xB8000
#define VIDEO_0 0xB9000
#define VIDEO_1 0xBA000
#define VIDEO_2 0xBB000
#define FOURKB 0x1000
#define FOURMB 0x400000  // 0000 0000 0100 0000 0000 0000 0000 0000
#define EIGHTMB 0x800000 // 0000 0000 1000 0000 0000 0000 0000 0000
#define USER_VIRT_ADDR 32
#define USER_VID_MEM_LOC 33

#define US_RW_P 7
#define US_RW 6
#define RW 2
#define RW_P 3
#define PS_RW_P 0x87


extern int open_terminal;


/* Init a page directory */
extern void blank_page_dir(unsigned int * page_dir);

/* Init a page table */
extern void blank_page_table(unsigned int * page_table);

/* Put page directory pointer in CR3 */
extern void loadPageDirectory(unsigned int * page_dir);

/* Inline assembly to set CR0 and CR4 as needed to enable paging */
extern void enablePaging();

/*
 * Wrapper on the above functions to enable paging given a single page
 * directory and page table
 */
extern void paging_init(unsigned int * page_dir, unsigned int * page_table);

/*
*	Allocates a new page for user-level programs
*/
extern void new_program_page(const int pid);

/*
*	Allocates a new 4kB video memory page for user-level programs
*/
extern void user_vid_mem();

/*
*	Copies and switches video memory
*/
extern int switch_vid_page(int dest);


#endif /* _PAGING_H */
