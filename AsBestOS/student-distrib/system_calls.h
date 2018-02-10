/* asbestOS -- a best OS*/

#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

#include "lib.h"
#include "types.h"
#include "rtc.h"
#include "filesystem.h"
#include "paging.h"
#include "kb.h"
#include "execute.h"


int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(uint32_t fd);

int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);


#endif
