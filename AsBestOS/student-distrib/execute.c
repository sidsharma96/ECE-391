/* asbestOS -- a best OS*/

#include "execute.h"

static int process_id[6];
char* arg_pointer;

/*
    int invalid_stdio()
    This function is an empty placeholder for the stdin/out empty slots
    Inputs: None
    Outputs: returns -1
    Return Value: -1
*/
int invalid_stdio()
{
    return -1;
}

/*
    char* get_exec_arg()
    This function returns the arg ptr for parse args on the user side
    Inputs: None
    Outputs: None
    Return Value: argument pointer
*/
char* get_exec_arg()
{
    return arg_pointer;
}

/*
    int get_term_proc()
    This function returns the terminal process currently running.
    Inputs: None
    Outputs: Empty process id array
    Return Value: terminal process currently running
*/
int get_term_proc()
{
    return terminal_proc_currently_running;
}

/* Global variables*/

general_ops_table_t stdin_jumptable = {term_open, term_close, 
                                                term_read, invalid_stdio};
general_ops_table_t stdout_jumptable = {term_open, term_close, 
                                                invalid_stdio, term_write};
int new_process_on_terminal[3] = {0, 0, 0};

int terminal_proc_currently_running = 0;

PCB* open_programs[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

int exception_flag = 0;


/*
    void execute_init()
    This function sets the initial process id array to 0.
    Inputs: None
    Outputs: Empty process id array
    Return Value: None
*/
void execute_init()
{
    int i;
    for(i = 0; i < 6; i++){
        process_id[i] = -1;  //ORIGINALLY 0
    }
}


/*
    int execute(const char * input)
    The exeute system call attempts to load and execute a new program, handing off the processor to the new program
	until it terminates.
    Inputs: Program needed to be executed
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int execute(const char * input)
{
    // parse args
    // \n\0 terminated string
    // char command[BUFSIZE];
    // char argument[BUFSIZE];
    char command[128] = {'\n', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    char argument[128] = {'\n', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    int child_pid;
    PCB* child_PCB = NULL;
    int ignore;
    int program_instruction_entry;
    arg_pointer = argument;
    
    if (input == NULL)
    {
        return -1;
    }

    int proc_requested = 0; 
    int idx = 0;
    for(idx = 0; idx < 6; idx++)
    {
        if(open_terminal == process_id[idx])
        {
            proc_requested++;
        }
    }
    if(proc_requested >= 4)
    {
        printf("Error: Starting this process does not leave room for shells on other terminals.\n");
        return -1;
    }
    // Parse the arguement 
    parse_args(input, command, argument);

    // printf("%s\n", command);
    // printf("--\n");

    // Try to get new process' ID
    PCB* cur_PCB = get_parent_pcb(open_terminal);

    child_pid = get_new_pid();

    if (child_pid == -1)
    {
        printf("No space for currently requested process.\n");
        return -1;
    }

    // Set up the new PCB
    child_PCB = (PCB*) (EIGHTMB - ((child_pid + 1) * EIGHTKB));
    current_pcb_ptr = cur_PCB;
    child_PCB->pid = child_pid;

    // Only set up the parent fields if calling execute from
    // an existing process.
    if ((int) cur_PCB >= (EIGHTMB - MAX_NUM_PROC * EIGHTKB) && cur_PCB != child_PCB)
    {
        child_PCB->parent_PCB = cur_PCB;
    }
    else
    {
        child_PCB->parent_PCB = NULL;
        current_pcb_ptr = child_PCB;
    }

    if(new_process_on_terminal[open_terminal] == 1)
    {
        child_PCB->parent_PCB = NULL;
        current_pcb_ptr = child_PCB;
        new_process_on_terminal[open_terminal] = 0;
    }

    // Always set up stdin and stdout
    child_PCB->open_files[0].file_ops_table_ptr = &stdin_jumptable;
    child_PCB->open_files[0].inode = -1;
    child_PCB->open_files[0].file_pos = -1;
    child_PCB->open_files[0].flags = 1;

    child_PCB->open_files[1].file_ops_table_ptr = &stdout_jumptable;
    child_PCB->open_files[1].inode = -1;
    child_PCB->open_files[1].file_pos = -1;
    child_PCB->open_files[1].flags = 1;

    // Set up User Space page and load program into memory
    new_program_page(child_pid);

    program_instruction_entry = loader(child_pid, command);

    // Program instruction = EIP, needed for the context switch
    if(program_instruction_entry == -1)
    {
        printf("Failed to load program executable.\n");
        new_program_page(current_pcb_ptr->pid);
        process_id[child_pid] = -1;

        return -1;
    }

    child_PCB->term_id = open_terminal;
    terminal_pcb_ptrs[open_terminal] = child_PCB;

    open_programs[child_PCB->pid] = child_PCB;

    child_PCB->cur_esp = USER_VIRT_STACK;
    child_PCB->cur_ebp = USER_VIRT_STACK;
    
    current_pcb_ptr = child_PCB;

    // prep for the context switch and iret
    tss.esp0 = (EIGHTMB - (child_pid * EIGHTKB) - 4);
    
    asm ("movl %1, %%ebx;"    /* ebx = a */
          "movl %%eax, %0;"   /* b = eax */
         :"=r"(ignore)        /* output */
         :"r"(program_instruction_entry)         /* input */
         :"%ebx"              /* clobbered register */
         );  

    register int esp_current_proc asm("esp");
    register int ebp_current_proc asm("ebp");

    if(current_pcb_ptr->parent_PCB != NULL)
    {
        current_pcb_ptr->parent_PCB->cur_esp = esp_current_proc;
        current_pcb_ptr->parent_PCB->cur_ebp = ebp_current_proc;
    }

    execute_wrapper(program_instruction_entry);

    register int eax asm("eax");
    return eax;
}

/*
    void parse_args(const char * input, char * command, char * argument)
    Helper function used to parse the arguement given to execute
    Inputs: 3 strings, needed to parse
    Outputs: Parsed string
    Return Value: None
*/

void parse_args(const char * input, char * command, char * argument)
{
    int position = 0;
    int cmdPos;

    while(input[position] != '\n' && input[position] != '\0')
    {
        if (input[position] == ' ')
        {
            position++;
            continue;
        }

        if (command[0] == '\n')
        {
            cmdPos = position;

            while(input[cmdPos] != ' ' && input[cmdPos] != '\n' && input[cmdPos] != '\0')
            {
                cmdPos++;
            }

            strncpy((int8_t *) command,
                    (const int8_t *) (&input[position]),
                    (cmdPos - position));

            position = cmdPos;
            continue;
        }

        if (argument[0] == '\n')
        {
            cmdPos = position;

            while(input[cmdPos] != ' ' && input[cmdPos] != '\n' && input[cmdPos] != '\0')
            {
                cmdPos++;
            }
            strncpy((int8_t *) argument,
                    (const int8_t *) (&input[position]),
                    (cmdPos - position));
        }
        position++;
    }
}

/*
    uint8_t is_executable(const char * command)
    Helper function used to check whether the given exexcutable is valid
    Inputs: executable name
    Outputs: 0 on success, 1 on failure
    Return Value: 8 bit integer idicating success or failure
*/

uint8_t is_executable(const char * command)
{
    char executable_check[EXEC_MAGIC_LEN];

    // Magic numbers provided to us in spec to check if a file is an executable
    char executable_magic[4] = {0x7F, 0x45, 0x4C, 0x46};

    int fd = open_file((const char*) command);

    // Reading file to executable check
    read_file(fd, executable_check, EXEC_MAGIC_LEN);
    close_file(fd);

    // If same value then success
    if (strncmp(executable_magic, executable_check, EXEC_MAGIC_LEN)) 
    {
        return 0;
    }

    return 1;
}


/*
    int get_new_pid()
    Helper function used to get next available process
    Inputs: None
    Outputs: Process id on success, 1 on failure
    Return Value: Integer idicating success or failure
*/
int get_new_pid()
{
    int i;
    for(i = 0; i < 6; i++)
    {
        if (process_id[i] == -1)
        {
            process_id[i] = open_terminal;
            return i;
        }
    }

    // No space for process
    return -1;
}

PCB* get_parent_pcb(int terminal)
{
    if(terminal > 2 || terminal < 0)
    {
        return NULL;
    }
    int i;
    for(i = 5; i > -1; i--)
    {
        if(process_id[i] == terminal)
        {
            return open_programs[i];
        }
    }

    // Failure
    return NULL;
}



/*
    int loader(const int new_pid, const char * command)
    Helper function used to retrieve the instruction pointer for context switching
    Inputs: Process id, command needed
    Outputs: EIP on success, -1 on failure
    Return Value: Integer idicating success or failure
*/
int loader(const int new_pid, const char * command)
{
    int num_bytes = 0;
    int read_result;        
    int fd;
    int addr_pt;

    // Load the program into memory
    fd = open_file(command);
    if(fd == -1)
        return -1;
    num_bytes = get_file_size(fd);
    read_result = read_file(fd, (int*) VIRTUAL_ADDR, num_bytes);
    close_file(fd);

    // Whether the file can be read or not
    if(read_result < 0)
    {
        printf("Failed to read file.\n");
        return -1;
    }
    else if(!is_executable(command))
    {
        printf("Not executable\n");
        return -1;
    }
    
    addr_pt = *((int*) (VIRTUAL_ADDR+24));
    return addr_pt;
}


/*
    int halt(uint8_t status)
    The halt system call terminates a process, returning the specifed value to its parent proess. 
    Inputs: 8 bit integer to indicate status
    Outputs: 0 on success, 1 on failure
    Return Value: Integer indicating success or failure
*/
int halt(uint8_t status)
{
    int fd;

    // Error checking
    current_pcb_ptr = terminal_pcb_ptrs[terminal_proc_currently_running];
    if (current_pcb_ptr == NULL)
    {
        return -1;
    }

    open_programs[current_pcb_ptr->pid] = NULL;
    process_id[current_pcb_ptr->pid] = -1; //ORIGINALLY 0

    // Close all open files
    for (fd = 2; fd < 8; fd++)
    {
        if(current_pcb_ptr->open_files[fd].flags)
        {
            close(fd);
        }
    }

    // If last process is shell, cannot be halted. 
    // Clear the screen and execute shell again.
    if(current_pcb_ptr->parent_PCB == NULL)
    {
        new_process_on_terminal[current_pcb_ptr->term_id] = 1;
        term_clear();
        execute("shell");
    }

    current_pcb_ptr = current_pcb_ptr->parent_PCB;
    terminal_pcb_ptrs[current_pcb_ptr->term_id] = current_pcb_ptr;

    // Set virtual page to old physical address
    new_program_page(current_pcb_ptr->pid);

    // Prepare for context switch
    tss.esp0 = EIGHTMB - (current_pcb_ptr->pid * EIGHTKB) - 4;
    tss.ss0 = KERNEL_DS;

    // Get BL for return value
    asm (   "movl %0, %%esp;"
            "movl %1, %%ebp;"
            :
            :"r"(current_pcb_ptr->cur_esp), "r"(current_pcb_ptr->cur_ebp)
        );
    if(exception_flag)
    {
        exception_flag = 0;
        asm volatile ( "movl $256, %eax;\n\t");
        
    }
    else
    {
        asm volatile (  "movl %ebx, %eax;\n\t"
                        "andl $0x000000FF, %eax;\n\t");

    }
    
    asm ("jmp halty_boi");        

    return 0;
}


/* Helper function to allow halt to return the value 256. */
void halt_exception()
{
    exception_flag = 1;
    halt(255);
}

/*
    int change_running_process(int term_id)
    This function changes the currently running process, but does not change the terminal being viewed.
    Inputs: new terminal id
    Outputs: changed process
    Return Value: 0 on success, -1 on failure
*/
int change_running_process(int term_id)
{
    if(term_id > 2 || term_id < 0)
    {
        term_write(0, "Bad term_id\n", 12);
        return -1;
    }

    register int esp_current_proc asm("esp");
    register int ebp_current_proc asm("ebp");

    current_pcb_ptr->cur_esp = esp_current_proc;
    current_pcb_ptr->cur_ebp = ebp_current_proc;

    if(terminal_pcb_ptrs[term_id] == NULL)
    {
        new_process_on_terminal[term_id] = 1;
        terminal_proc_currently_running = term_id;
        execute("shell");
    }
    else
    {
        terminal_proc_currently_running = term_id;

        current_pcb_ptr = terminal_pcb_ptrs[term_id];
        new_program_page(current_pcb_ptr->pid);

        //Preparing for context switch
        tss.esp0 = EIGHTMB - (current_pcb_ptr->pid * EIGHTKB) - 4;
        tss.ss0 = KERNEL_DS;

        asm (   "movl %0, %%esp;"
            :
            :"r"(current_pcb_ptr->cur_esp)
        );
        asm (   "movl %0, %%ebp;"
            :
            :"r"(current_pcb_ptr->cur_ebp)
        );
        
        asm volatile("leave;\n"
                    "ret\n");        
    }

    return 0;
}

/*
    void schedule_next_proc()
    This function gets the next process to be run, then calls the function to change the process.  This gets called from the PIT interrupt
    Inputs: None
    Outputs: None
    Return Value: None
*/
void schedule_next_proc()
{
    int i = terminal_proc_currently_running + 1;
    
    while(terminal_pcb_ptrs[i % 3] == NULL)
    {
        i = (i + 1) % 3;
    }

    i = (i % 3);
    change_running_process(i);
}

