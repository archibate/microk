#pragma once

#include <struct.h>
/*UNION(FS_ARGS)
{
	char __[20];
	struct {
		union {
			uint sysnr;
			int ret_fd;
			long rw_size;
		};
		union {
			struct {
				char file_name[16];
			};
			struct {
				uint arg_fd;
				union {
					struct {
						char tx_buf[12];
					};
				};
			};
		};
	};
};*/
STRUCT(FOP_ARGS)
{
	union {
		enum {
			FOP_READTX,
			FOP_WRITETX,
		} fopnr;
		long rw_size;
	};
	char tx_buf[16];
};
