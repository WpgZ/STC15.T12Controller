#ifndef SYSTEM_H_20160714
#define SYSTEM_H_20160714


#if TARGET_CHIP == STC15F204

#include "../GENERATED/XSTC15F204EA.h"

#endif

#if defined SDCC
#include "../../build/sdcc/include/SDCC.h"
#endif

#if defined __CX51__
#include "../../build/keil/include/KEIL.h"
#endif

#endif
