/* asbestOS -- a best OS*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "types.h"
#include "lib.h"
#include "execute.h"

#define DENTRY_SIZE 64
#define BLOCK_BYTE_SIZE 4096
#define STRING_NAME_SIZE 32
#define FD_TABLE_SIZE 8


typedef struct inode{
    int length;
    int db[1023];
} inode;


typedef struct dentry{
    char filename[32];
    int type;
    int nodeIndex;//inode
    char padding[24];
} dentry_t;


typedef struct bootBlock{
    int numDir;
    int numInodes;
    int databblocks;
    char padding[52];
    dentry_t dentry[63];//there are numDir entries
} bootBlock;


int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

void filesystem_init(uint32_t fs_ptr);

int32_t get_file_size_from_inode(uint32_t inode_number);
int32_t get_file_size(int fd);

int32_t find_open_desc_index();

int32_t open_file(const char *path);
int32_t open_dir(const char *path);
int32_t read_file(int32_t fd, void* buf, int32_t num_bytes);
int32_t read_dir(int32_t fd, void* buf, int32_t num_bytes);
int32_t write_file(int32_t fd, void* buf, int32_t num_bytes);
int32_t write_dir(int32_t fd, void* buf, int32_t num_bytes);
int32_t close_file(int32_t fd);
int32_t close_dir(int32_t fd);


#endif
