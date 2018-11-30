#pragma once

enum {
	FS_OPEN,
	FS_READ,
};

#include <struct.h>
UNION(FS_ARGS)
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
		}
	};
};
