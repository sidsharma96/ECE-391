/* asbestOS -- a best OS*/

#include "filesystem.h"

inode* inode_start;
void* datablock_start;
bootBlock* boot_block;
static general_ops_table_t file_ops_table_desc = {open_file, close_file, 
                                                    read_file, write_file};
static general_ops_table_t dir_ops_table_desc = {open_dir, close_dir, 
                                                    read_dir, write_dir};


/*
    int32_t read_dentry_by_name
    This function finds the dentry corresponding to a file's name
    passed in as a pointer.
    Inputs: The name of a file and a pointer for a dentry to be copied into.
    Outputs: 0 on success, 1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
    int i;
    char* entryName;
    dentry_t dt;
    if(dentry == NULL || fname == NULL)
        return -1;
    for(i = 0; i < boot_block->numDir; i++)
    {
        dt = boot_block->dentry[i];
        entryName = dt.filename;

        // Iterate through the boot block until the entry has been found, then copy it into the dentry pointer.
        if(strncmp((const int8_t *)entryName, (const int8_t *)fname, STRING_NAME_SIZE) == 0)
        {
            memcpy((dentry_t *)dentry, (dentry_t *) (&dt), (uint32_t)DENTRY_SIZE);
            return 0;
        }
    }
    return -1;
}


/*
    int32_t read_dentry_by_index
    Given an index into to the boot block, this function copies the corresponding
    dentry into the dentry buffer pointer passed in as an argument.
    Inputs: index into the boot block, a dentry buffer pointer
    Outputs: 0 on success, 1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
    if(index >= boot_block->numDir || dentry == NULL)
        return -1;
    dentry_t dt = boot_block->dentry[index];
    memcpy((dentry_t *)dentry, (dentry_t *) (&dt), (uint32_t)DENTRY_SIZE);
    return 0;
}


/*
    int32_t read_data
    Given an inode index, the lenght of the data block, and on offset from that index, this function copies the data
    from that location in storage and copies it into the buffer passed in as an operand.
    It returns the number of bytes copied.
    Inputs: inode index, offset from base of inode, buffer to copy into, length of data block
    Outputs: Number of bytes copied or failure.
    Return Value: -1 on failure, any non-negative value for success.
*/
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
    struct inode* inode_block_ptr;
    int cur_block_idx, first_block_offset;
    int file_size_bytes;
    int lengthCopied = 0;
    int bufLoc;
    int dataBlock;
    int write_pos = 0;
    int copy_size = 0;
    if (inode >= boot_block->numInodes)
    {
        return -1;
    }

    // Validate inode index
    inode_block_ptr = (struct inode *) ((int)inode_start + inode * BLOCK_BYTE_SIZE);
    file_size_bytes = inode_block_ptr->length;
    
    if(offset >= file_size_bytes)
    {
        return 0;
    }
    if (buf == NULL || length == 0)
    {
        return -1;
    }
    
    if (length + offset > file_size_bytes)
    {
        length = file_size_bytes - offset;
    }
    
    // cur_block_idx validated by ensuring (offset <= file_size_bytes)
    cur_block_idx = offset / BLOCK_BYTE_SIZE;
    first_block_offset = offset % BLOCK_BYTE_SIZE;

    dataBlock = inode_block_ptr->db[cur_block_idx];    

    if (first_block_offset > 0)
    {
        bufLoc = (int)datablock_start + (dataBlock * BLOCK_BYTE_SIZE) + first_block_offset;
        copy_size = BLOCK_BYTE_SIZE - first_block_offset;
        if(copy_size > length)
            copy_size = length;
        memcpy((char *) buf, (char *) bufLoc, copy_size);
        write_pos += copy_size;
        lengthCopied += copy_size;
        cur_block_idx++;
    }

    while (lengthCopied < length)
    {
        dataBlock = inode_block_ptr->db[cur_block_idx];
        bufLoc = (int) datablock_start + dataBlock * BLOCK_BYTE_SIZE;
        if(length - lengthCopied >= BLOCK_BYTE_SIZE)
        {
            memcpy((char *) (&buf[write_pos]), (char *) bufLoc, (BLOCK_BYTE_SIZE));
            lengthCopied += BLOCK_BYTE_SIZE;
            write_pos += BLOCK_BYTE_SIZE;
        }
        else
        {
            memcpy((char *) (&buf[write_pos]), (char *) bufLoc, (length - lengthCopied));    
            lengthCopied += length - lengthCopied;
        }
        cur_block_idx++;
    }

    return lengthCopied;
}


/*
    void filesystem_init
    Given a pointer to a file system, this function sets up the global variables for
    the file system handler document. In doing so, the file system is initialized and
    ready for viewing.
    Inputs: A pointer to a file system.
    Outputs: None
    Return Value: None
*/
void filesystem_init(uint32_t fs_ptr)
{
    boot_block = (bootBlock *) fs_ptr;
    inode_start = (inode *) (fs_ptr + BLOCK_BYTE_SIZE);
    datablock_start = (void *) ((uint32_t) inode_start + (boot_block->numInodes) * BLOCK_BYTE_SIZE);
}


/*
    int32_t get_file_size_from_inode
    This function receives an inode index as an argument and returns the size of
    the file corresponding to that index in bytes.
    Inputs: An inode index
    Outputs: The size of that data block, or -1 on failure
    Return Value: Failure or a file size.
*/
int32_t get_file_size_from_inode(uint32_t inode_number)
{
    struct inode* inode_block_ptr;

    if(inode_number >= boot_block->numInodes)
        return -1;
    // check that inode index is valid

    inode_block_ptr = (struct inode *) ((int)inode_start + inode_number * BLOCK_BYTE_SIZE);

    return inode_block_ptr->length;
}


/*
    int32_t get_file_size
    This function receives a FD as an argument and returns the size of
    the file corresponding to that index in bytes, by calling get_file_size_from_inode
    Inputs: An inode index
    Outputs: The size of that data block, or -1 on failure
    Return Value: Failure or a file size.
*/
int32_t get_file_size(int fd)
{
    if(fd < FD_TABLE_SIZE && current_pcb_ptr->open_files[fd].flags == 0)
    {
        return -1;
    }

    return get_file_size_from_inode(current_pcb_ptr->open_files[fd].inode);
}


/*
    int32_t open_file
    Given a file path as an argument, this function marks that file as being currently
    open and ready for viewing.
    Inputs: A pointer to a file path
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t open_file(const char * path)
{
    int retVal;
    dentry_t current_dentry;

    if(path == NULL)
    {
        return -1;
    }

    retVal = read_dentry_by_name((const uint8_t *)path, (dentry_t *) (&current_dentry));

    if(retVal < 0 || current_dentry.type != 2)
    {
        return -1;
    }

    retVal = find_open_desc_index();

    if(retVal == -1)
    {
        return retVal;
    }

    current_pcb_ptr->open_files[retVal].file_ops_table_ptr = &file_ops_table_desc;
    current_pcb_ptr->open_files[retVal].inode = current_dentry.nodeIndex;
    current_pcb_ptr->open_files[retVal].file_pos = 0;
    current_pcb_ptr->open_files[retVal].flags = 1;

    return retVal;
}


/*
    int32_t find_open_desc_index
    Finds and returns the first open index in the file descriptor table, if any.
    Inputs: none
    Outputs: -1 on failure, index on success
    Return Value: A 32bit integer describing a valid descriptor index or failure.
*/
int32_t find_open_desc_index()
{
    int i;

    for(i = 0; i < FD_TABLE_SIZE; i++)
    {
        if(current_pcb_ptr->open_files[i].flags == 0)
        {
            return i;
        }
    }

    return -1;
}


/*
    int32_t open_dir
    Given a file path as an argument, this function marks that directory as being currently
    open and ready for viewing.
    Inputs: A pointer to a file path
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer with the current descriptor index.
*/
int32_t open_dir(const char * path)
{
    int retVal;
    dentry_t current_dentry;
    if(path == NULL)
    {
        return -1;
    }
    
    retVal = read_dentry_by_name((const uint8_t *)path, (dentry_t *) (&current_dentry));
    if(retVal < 0 || current_dentry.type != 1)
    {
        return -1;
    }

    retVal = find_open_desc_index();

    if(retVal == -1)
    {
        return retVal;
    }

    current_pcb_ptr->open_files[retVal].file_ops_table_ptr = &dir_ops_table_desc;
    current_pcb_ptr->open_files[retVal].file_pos = 0;
    current_pcb_ptr->open_files[retVal].flags = 1;

    return retVal;
}

/*
    int32_t read_file
    Given a file descriptor as an argument, this function copies num_bytes from the
    file specified by the descriptor into the buffer whose address is specified by
    the void pointer buf.
    Inputs: File descriptor, pointer to an output buffer, the number of bytes to copy
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t read_file(int32_t fd, void * buf, int32_t num_bytes)
{
    int bytesRead = 0;
    if(buf == NULL)
    {
        return -1;
    }

    // Nothing in fd
    if(fd < FD_TABLE_SIZE && current_pcb_ptr->open_files[fd].flags == 0)
    {
        return -1;
    }

    bytesRead = read_data(current_pcb_ptr->open_files[fd].inode, current_pcb_ptr->open_files[fd].file_pos, (uint8_t*)buf, num_bytes);
    current_pcb_ptr->open_files[fd].file_pos += bytesRead;

    return bytesRead;
}

/*
    int32_t read_dir
    Given a file descriptor as an argument, this function copies num_bytes from the
    directory specified by the descriptor into the buffer whose address is specified by
    the void pointer buf.
    Inputs: File descriptor, pointer to an output buffer, the number of bytes to copy
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t read_dir(int32_t fd, void * buf, int32_t num_bytes)
{
    char* entryName;
    dentry_t current_dir_dentry;    

    // Nothing in fd
    if(fd < 0 || fd >= FD_TABLE_SIZE || current_pcb_ptr->open_files[fd].flags == 0)
    {
        return -1;
    }

    if(buf == NULL)
    {
        return -1;
    }

    if(current_pcb_ptr->open_files[fd].file_pos >= boot_block->numDir)
    {
        return 0;
    }

    if(read_dentry_by_index(current_pcb_ptr->open_files[fd].file_pos, (dentry_t *) (&current_dir_dentry)) == -1)
    {
        return -1;
    }

    if(num_bytes > 32)
    {
        num_bytes = 32;
    }

    current_pcb_ptr->open_files[fd].file_pos++;
    entryName = current_dir_dentry.filename;
    memcpy((char *)buf, (char *) (entryName), (uint32_t)num_bytes);

    return num_bytes;
}

/*
    int32_t write_file
    Given a file descriptor as an argument, this function copies num_bytes from the
    buffer whose address is specified by the void pointer buf into the file which is
    specified by the file descriptor fd.
    Inputs: File descriptor, pointer to an input buffer, the number of bytes to copy
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t write_file(int32_t fd, void * buf, int32_t num_bytes)
{
    return -1;
}

/*
    int32_t write_file
    Given a file descriptor as an argument, this function copies num_bytes from the
    buffer whose address is specified by the void pointer buf into the directory which is
    specified by the file descriptor fd.
    Inputs: File descriptor, pointer to an input buffer, the number of bytes to copy
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t write_dir(int32_t fd, void * buf, int32_t num_bytes)
{
    return -1;
}

/*
    int32_t close_file
    Given a file descriptor, this function marks the corresponding file
    as being closed and not ready for viewing.
    Inputs: a file descriptor
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t close_file(int32_t fd)
{
    if(fd < FD_TABLE_SIZE && current_pcb_ptr->open_files[fd].flags)
    {
        current_pcb_ptr->open_files[fd].flags = 0;
        return 0;
    }

    return -1;
}

/*
    int32_t close_dir
    Given a file descriptor, this function marks the corresponding directory
    as being closed and not ready for viewing.
    Inputs: none
    Outputs: 0 on success, -1 on failure
    Return Value: A 32bit integer describing success or failure.
*/
int32_t close_dir(int32_t fd)
{
    if(fd < FD_TABLE_SIZE && current_pcb_ptr->open_files[fd].flags)
    {
        current_pcb_ptr->open_files[fd].flags = 0;
        return 0;
    }

    return -1;
}
