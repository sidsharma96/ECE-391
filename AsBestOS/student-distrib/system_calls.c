/* asbestOS -- a best OS*/

#include "system_calls.h"


/*
    int32_t read(int32_t fd, void* buf, int32_t nbytes)
    The read system call reads data from the keyboard, a file, device (RTC), or directory. This all returns the number
    of bytes read. 
    Inputs: File discriptor, buffer, number of bytes to be read
    Outputs: Number of bytes read on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
    if(fd < 0 || fd >= FD_TABLE_SIZE)
    {
        return -1;
    }
    else if(current_pcb_ptr->open_files[fd].flags == 1)
    {
        return ((current_pcb_ptr->open_files[fd]).file_ops_table_ptr)->read(fd, buf, nbytes);
    }

    return -1;
}


/*
    int32_t write(int32_t fd, const void* buf, int32_t nbytes)
    The write system call writes data to the terminal or to a device (RTC).
    Inputs: File discriptor, buffer, number of bytes to be written
    Outputs: 0Number of bytes written on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{    
    if(fd < 0 || fd >= FD_TABLE_SIZE)
    {
        return -1;
    }

    if(current_pcb_ptr->open_files[fd].flags == 1)
    {
        return ((current_pcb_ptr->open_files[fd]).file_ops_table_ptr)->write(fd, buf, nbytes);
    }

    return -1;
}


/*
    int32_t open(const uint8_t * filename)
    The open system call provides access to the file system. The call finds the directory entry corresponding to the 
    named file, allocates an unused file desriptor, and sets up any data neessary to handle the given type of file.
    Inputs: Filename of file to be opened
    Outputs: Open sys call for each file type on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t open(const uint8_t * filename)
{
    uint32_t de[sizeof(dentry_t)];
	dentry_t *file_dentry = (dentry_t *)(&de);

    if(-1 == read_dentry_by_name(filename, file_dentry))
    {
        return -1;
    }

    switch(file_dentry->type)
    {
        case 0:  // RTC
            return rtc_open(filename);
        case 1:  // Directory
            return open_dir((char *) filename);
        case 2:  // File
            return open_file((char *) filename);
        default:
            return -1;
    }

    return -1;
}


/*
    int32_t close(uint32_t fd)
    The close system all closes the specified file descriptor and makes it available 
    for return from later calls to open.
    Inputs: File descriptor
    Outputs: Close sys call on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t close(uint32_t fd)
{
    if(fd < 0 || fd >= FD_TABLE_SIZE)
    {
        return -1;
    }

    if(current_pcb_ptr->open_files[fd].flags == 1)
    {
        return ((current_pcb_ptr->open_files[fd]).file_ops_table_ptr)->close(fd);
    }

    return -1;
}

/*
    int32_t getargs(uint8_t* buf, int32_t nbytes)
    This function obtains the argument that was passed into the execute system call
    and returns it to the caller.
    Inputs: Buffer size, number of bytes needed.
    Outputs: Success, failure
    Return Value: -1, 0
*/

int32_t getargs(uint8_t* buf, int32_t nbytes)
{
    // Obtain a pointer to the argument in the execute.c file
    char* args = (char*) get_exec_arg();
    int i;

    if(buf == NULL || nbytes < 1)
    {
        return -1;
    }

    // Copy the argument into the user level buffer
    for(i = 0; i < nbytes && i < 64; i++)
    {
        buf[i] = (uint8_t) args[i];
    }

    if(buf[0] == '\n' || buf[0] == '\0')
    {
        return -1;
    }
    
    return 0;
}



/*
    int32_t vidmap(uint8_t** screen_start)
    This function creates a new user-level page to interface with video memory,
    as well as set the user start of video memory to the appropriate page.
    Inputs: Start of video memory.
    Outputs: -1, new address
    Return Value: -1, new address
*/

int32_t vidmap(uint8_t** screen_start)
{
    unsigned int vid_addr = FOURMB * USER_VID_MEM_LOC + (open_terminal * FOURKB);
    
    // Check for bad pointers
    if((screen_start == NULL) || (screen_start < (uint8_t**) 0x08000000) || (screen_start >= (uint8_t**) (0x08000000 + FOURMB)))
    {
        return -1;
    }
    
    // Allocate a new page and set the pointer to its address
    user_vid_mem();
    *screen_start = (uint8_t*)vid_addr;

    return vid_addr;
}
