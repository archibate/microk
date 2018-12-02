#pragma once

#define L4_SEND  0
#define L4_WAIT  1
#define L4_CALL  2
#define L4_RETN  3
#define L4_FORK  4
#define L4_ACTV  5
#define L4_SHARE 6
#define L4_GRANT 7
#define L4_REAL  8
#define L4_ANY   255
#define L4_NIL   255
#define MAXMSG   16
#define MAXIRQ   16
#define MAXTASK  128
#define L4_IRQ(irq)  (MAXTASK + (irq))

#include <types.h>

typedef int l4id_t;
typedef uchar cap_t;
