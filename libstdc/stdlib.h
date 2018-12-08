#pragma once

#include <stddef.h>
#include <malloc.h>

void __attribute__((noreturn)) exit(int res);
void __attribute__((noreturn)) abort(void);
