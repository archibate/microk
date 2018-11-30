#pragma once

#include <types.h>
#include <struct.h>
#include "caps.h"

#define MAXCAP 256
typedef uchar cap_t; // auto warp for MAXCAP = 256
STRUCT(CSPACE) { // sized 4096 bytes -- exactly one page
	CAP C[MAXCAP];
};
