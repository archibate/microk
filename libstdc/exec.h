#pragma once

#include <sys/proc.h> // execa
#define exec(path) execa(path, "")
