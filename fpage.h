#pragma once

STRUCT(FPAGE)
{
	FPAGE *next;
	union {
		struct {
			ulong base;
			uint shift : 16;
			uint mpid : 6;
			uint flags : 6;
			uint rwx : 4;
		};
		uint raw[2];
	};
};
