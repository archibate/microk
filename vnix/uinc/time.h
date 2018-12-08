#pragma once

#include <inttypes.h>

typedef uint time_t;

typedef struct _dostime {
	uint dt_hour  : 5;
	uint dt_min   : 6;
	uint dt_sec2  : 5;
} __attribute__((packed)) dostime_t;

typedef struct _dosdate {
	uint da_year  : 7;
	uint da_month : 4;
	uint da_day   : 5;
} __attribute__((packed)) dosdate_t;
