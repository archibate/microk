#pragma once

#ifndef DBG
#define dbg_printf(...)
#else
#define dbg_printf(...) printf(__VA_ARGS__)
#endif
