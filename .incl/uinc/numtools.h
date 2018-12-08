#pragma once

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define HIBYTE(x) (((x)>>8)&0xff)
#define LOBYTE(x) ((x)&0xff)
#define MKWORD(l,h) (((l)&0xff)|(((h)&0xff)<<8))
#define MKDWORD(l,h) (((l)&0xffff)|(((h)&0xffff)<<16))
#define HIWORD(x) (((x)>>16)&0xffff)
#define LOWORD(x) ((x)&0xffff)
