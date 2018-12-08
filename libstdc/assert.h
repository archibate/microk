#pragma once

int __attribute__((noreturn)) __assertion_failure(const char *expr);
#define assert(x) do { if (!(x)) __assertion_failure(#x); } while (0)
