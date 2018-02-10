/* asbestOS -- a best OS*/

#ifndef PIT_H
#define PIT_H

#include "lib.h"
#include "i8259.h"
#include "execute.h"


#define PIT_P1      0x43
#define PIT_P2      0x40

#define DATA1   0x34
#define PIT_FREQ    1193181
#define PIT_IRQ_NUM 0
#define VALUE_SET 50


void pit_init();
void pit_handler();


#endif
