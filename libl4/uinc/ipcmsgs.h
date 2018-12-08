#pragma once

#include <struct.h>
#include <inttypes.h>

STRUCT(ICH_MSG)
{
	union {
		uint ich;
		char ic_raw[20];
	};
};

STRUCT(TX_MSG)
{
	uchar tx_len;
	char tx_data[23];
};
