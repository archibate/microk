#pragma once

#define VHIM_SEC(...) __attribute__((".vhim" __VA_ARGS__)
#define VHIM_TEXT  VHIM_SEC(".text")
#define VHIM_DATA  VHIM_SEC(".data")
