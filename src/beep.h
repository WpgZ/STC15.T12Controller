#ifndef __BEEP_H__
#define __BEEP_H__
#include "typedef.h"

#define BEEP_OFF		0x00
#define BEEP_ONCE		0x01
#define BEEP_THREE		0x03
#define BEEP_CONTINUE	0x08

#define BEEP_SHORT		0x10
/*
 #define BEEP_NN			0x50
 #define BEEP_NM			0x60
 #define BEEP_NS			0x70

 #define BEEP_MN			0x90
 #define BEEP_MM			0xA0
 #define BEEP_MS			0xB0

 #define BEEP_SN			0xD0
 #define BEEP_SM			0xE0
 #define BEEP_SS			0xF0

 #define BEEP_H			0xC0
 #define BEEP_L			0x30*/
#define BEEP_NUMBER		0x07

//void BeepInit(int8 mode);
void Beep(void);
#endif
