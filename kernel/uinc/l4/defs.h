#pragma once

#define L4_SEND  0
#define L4_RECV  1
//#define L4_CALL  2
//#define L4_RPLY  3
#define L4_FORK  4
#define L4_ACTV  5
#define L4_CMAP  6
#define L4_MKCAP 7
#define L4_PRINT 13
#define L4_PUTS  14
#define L4_HALT  15
#define L4_ANY   255
#define L4_NIL   255
#define MAXTASK  128
#define MAXCAP   256
#define MAXIRQ   16
#define MAXMSG   16
#define L4_BLOCK 1
#define L4_ISCAP 2
#define L4_REPLY 4
#define L4_ISRW  2
#define PGSIZE 4096L
#define L4_IRQ(irq)   (MAXTASK + (irq))
#define L4_AXSYS(ax) ((ax) & 0xf)
#define L4_AXSTG(ax) (((ax) >> 4) & 0xf)
#define L4_STAGE(stg) (((stg) & 0xf) << 4)

#include <types.h>

typedef int l4id_t;
typedef uchar cap_t;
typedef uchar stage_t;
