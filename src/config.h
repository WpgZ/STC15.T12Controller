#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <mach/mcu51.h>

#include <typedef.h>

//#include "INTRINS.h"

#define S_TEMP_T12			0
#define S_PASS				1
#define S_VREF				2
#define S_PID_P				3
#define S_PID_I				4
#define S_PID_D				5
#define S_EDT				6
#define S_VOS				7
#define S_TEMP_COMP			8
#define S_TEMP_HIBER		9
#define S_TIME_HIBER		10
#define S_TIME_SHUTDOWN		11
#define S_NOINPUT_TIMEOUT	12
#define S_TEMP_ADJUST		13
#define S_PASS_TIMEOUT		14
#define S_SAVE_LOAD			15

#define S_PROTECTED			2
#define S_FIXED				1
#define S_FREE				0
#define S_UNDEFINED			0

#define S_EMPTY_PASS		555
#define S_UNDEFINED			0
#define S_TIME_EXHEATING 	0xFF

#define SETTING_NUM			16

extern int32_t	LED_RTT;
extern int32_t	Tsetting;
extern uint8_t	PID_P, PID_I, PID_D;
extern uint8_t	Vos;
extern int8_t		Tcomp;
extern int32_t	GainEx;
extern int32_t	Vref;

extern BIT PRE_HIBER_STATE;

void Config(void);
void ReadConfigs(void);
#endif