#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <mach/mcu51.h>

#include <typedef.h>

#define ALL_ONOFF			0x0080
#define CALC_DIGS			0x0040
#define FLASH_BITMASK		0x0030
#define DIG_ONE				0x0001
#define DIG_TEN				0x0002
#define DIG_HUNDRED			0x0004
#define FLASH_OFF_HALF		0x0008
#define DP_1				0x0100
#define DP_2				0x0200
#define DP_3				0x0300

#define DIG_ALL				0x0007
#define CTRL_DEF			0x00C0
#define FLASH_1000			0x0010
#define FLASH_500			0x0020
#define FLASH_333			0x0030
#define DP_MASK				0x0300

extern uint32_t	led_ctrl;
extern uint8_t  digits[3];
extern int32_t* led_value; 
void DisplayCalcDigits(int32_t a);
void Display(int32_t a);
void DisplayInit(BIT on, uint32_t ctrl);

#endif