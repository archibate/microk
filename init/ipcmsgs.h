#pragma once

#include <struct.h>
#include <types.h>

STRUCT(ICH_MSG)
{
	uint ich;
	char ic_reserved[16];
};

STRUCT(TX_MSG)
{
	uchar tx_len;
	char tx_data[19];
};
