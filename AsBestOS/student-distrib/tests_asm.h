#ifndef TESTS_ASM_H
#define TESTS_ASM_H

#include "types.h"
#include "lib.h"

extern int32_t syscalls_test_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t syscalls_test_write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t syscalls_test_close (int32_t fd);
extern int32_t syscalls_test_open (void* buf);

#endif
