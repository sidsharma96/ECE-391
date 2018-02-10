/* asbestOS -- a best OS*/

#ifndef EXECUTE_H
#define EXECUTE_H

#include "types.h"
#include "lib.h"
#include "filesystem.h"
#include "kb.h"
#include "rtc.h"
#include "x86_desc.h"
#include "paging.h"
#include "execute_asm.h"
#include "system_calls.h"


#define PRGRM_IMG_OFFSET 0x00048000
#define EXEC_MAGIC_LEN 4
#define BUFSIZE 32
#define VIRTUAL_ADDR  	0x08048000

#define EIGHTKB 		0x2000
#define MASK_8KB 		0xFFFFE000
#define USER_VIRT_STACK 0x083FFFFC
#define USER_VIRT_PAGE  0x08000000

#define MAX_NUM_PROC 6


/* General purpose table for open, read, write, close*/
typedef struct general_ops_table{
    int32_t (*open)();
    int32_t (*close)();
    int32_t (*read)();
    int32_t (*write)();
} general_ops_table_t;


/*file descriptor table needed to fill in PCB*/
typedef struct file_descriptor{
    general_ops_table_t* file_ops_table_ptr;
    int inode;
    int file_pos;
    int flags;
} file_descriptor_t;


/*PCB struct which contains the necessary control blocks and states of every program*/
typedef struct PCB_t{
	int pid;
	int cur_esp;
	int cur_ebp;
	file_descriptor_t open_files[8];
    int term_id;

	struct PCB_t * parent_PCB;
} PCB;

// struct PCB_t * current_pcb_ptr;
PCB* open_programs[6];
PCB* current_pcb_ptr;
int terminal_proc_currently_running;
int exception_flag;

PCB* terminal_pcb_ptrs[3];

int32_t invalid_stdio();
char* get_exec_arg();
int execute(const char * input);
void parse_args(const char * input, char * command, char * argument);
uint8_t is_executable(const char * command);
void execute_init();
int get_new_pid();
int loader(const int new_pid, const char * command);
int change_running_process(int term_id);
void schedule_next_proc();
int get_term_proc();
void halt_exception();
PCB* get_parent_pcb(int terminal);


#endif
