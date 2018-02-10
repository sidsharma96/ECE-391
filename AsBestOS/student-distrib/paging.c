/* asbestOS -- a best OS */

#include "paging.h"


/* Used this page as reference http://wiki.osdev.org/Setting_Up_Paging */

/* File-scope variables */
static unsigned int * page_dir_ptr;
static unsigned int * page_table_ptr;
unsigned int vid[NUM_ENTRIES] __attribute__((aligned(TABLE_SIZE)));
unsigned int vid_pages[3] = {VIDEO_0, VIDEO_1, VIDEO_2};


/* Function definitions */

/*
 * Initializes all entries in a page directory as not present
 * but read+write.
 *
 * Input: pointer to head of a page directory
 */
extern void blank_page_dir(unsigned int * page_dir)
{
    int idx;

    for (idx = 0; idx < NUM_ENTRIES; idx++)
    {
        page_dir[idx] = RW;
    }
}


/*
 * Initializes all entries in a page table as not present
 * but read+write and user privilege.
 *
 * Input: pointer to head of a page table
 */
extern void blank_page_table(unsigned int * page_table)
{
    int idx;

    for (idx = 0; idx < NUM_ENTRIES; idx++)
    {
        page_table[idx] = RW;
    }
}


/*
 * Loads the address of the page directory into CR3
 *
 * Input: pointer to head of a page directory
 */
extern void loadPageDirectory(unsigned int * page_dir)
{
    asm volatile (
        "movl    8(%ebp), %eax\n\t"
        "movl    %eax, %cr3"
    );
}


/*
 * Set PSE in CR4 to enable 4MB paging (bit 4)
 * Then set PG in CR0 to enable paging (bit 31)
 */
extern void enablePaging()
{
    asm volatile (
        "movl    %cr4, %eax\n\t"
        "orl     $0x00000010, %eax\n\t"
        "movl    %eax, %cr4"
    );


    asm volatile (
        "movl    %cr0, %eax\n\t"
        "orl     $0x80000000, %eax\n\t"
        "movl    %eax, %cr0\n\t"
    );
}


/*
 * Wrapper on the above functions to enable paging given a single page
 * directory and page table
 */
extern void paging_init(unsigned int * page_dir, unsigned int * page_table)
{
    blank_page_dir(page_dir);
    blank_page_table(page_table);

    page_table[VIDEO / TABLE_SIZE] = VIDEO | RW_P;
    page_table[VIDEO_0 / TABLE_SIZE] = VIDEO_0 | RW_P;
    page_table[VIDEO_1 / TABLE_SIZE] = VIDEO_1 | RW_P;
    page_table[VIDEO_2 / TABLE_SIZE] = VIDEO_2 | RW_P;

    page_dir[0] = ((unsigned int) page_table) | RW_P;

    /* Designate the 4MB page to start at 4MB (and thus extend to 8MB) */
    page_dir[1] = (1 << 22) | PS_RW_P;

    loadPageDirectory(page_dir);
    enablePaging();
    page_dir_ptr = page_dir;
    page_table_ptr = page_table;
}


/*
 * Helper function to initialize new page in execute, when virtual memory needs to
 * be translated into physical memory.
 */
extern void new_program_page(const int pid)
{
    int page_base_addr;
    int page_dir_entry;

    page_base_addr = EIGHTMB + (pid * FOURMB);

    // Set lower bits of physical address according to privilege level
    page_dir_entry = 0 | page_base_addr | PS_RW_P; 

    // Set PDE
    page_dir_ptr[USER_VIRT_ADDR] = page_dir_entry;

    // Flush TLB
    asm volatile (
        "movl    %cr3, %eax\n\t"
        "movl    %eax, %cr3"
    );
}


/*
    void user_vid_mem()
    This function is a helper function to inititialize the new video memory page for use in the vidmap system call.
    Inputs: none
    outputs: void, allocates vid memory page, flushes TLB
*/
extern void user_vid_mem()
{
    if(open_terminal == terminal_proc_currently_running)       
    {
        vid[terminal_proc_currently_running] = VIDEO | US_RW_P;
    }
    else
    {
        vid[terminal_proc_currently_running] = vid_pages[terminal_proc_currently_running] | US_RW_P;
    }
    
    page_dir_ptr[USER_VID_MEM_LOC] = ((unsigned int) vid) | US_RW_P;

    // Flush TLB
    asm volatile (
        "movl    %cr3, %eax\n\t"
        "movl    %eax, %cr3"
    );
}


/*
    int switch_vid_page(int dest)
    This is a helper function to clear and reload the screen when the terminals are switched
    inputs: destination terminal
    Return value: 0 on success, -1 on failure
*/
extern int switch_vid_page(int dest)
{
    if(dest > 2 || dest < 0)
    {
        return -1;
    }
    if(dest == open_terminal)
    {
        return 0;
    }

    cli();
    vid[open_terminal] = vid_pages[open_terminal] | US_RW_P;

    page_table_ptr[vid_pages[open_terminal] / TABLE_SIZE] = vid_pages[open_terminal] | RW_P;
    memcpy((unsigned int*) vid_pages[open_terminal], (unsigned int*) VIDEO, FOURKB);
    memcpy((unsigned int*) VIDEO, (unsigned int*) vid_pages[dest], FOURKB);
    page_table_ptr[vid_pages[dest] / TABLE_SIZE] = VIDEO | RW_P;

    vid[dest] = VIDEO | US_RW_P;
    open_terminal = dest;
    sti();

    return 0;
}
