#ifndef __ROTARYENCODER_H__
#define __ROTARYENCODER_H__
#include "typedef.h"

#define ENCODER_CW			0x01
#define ENCODER_CCW			0x02
#define ENCODER_SHORT_DOWN	0x04
#define ENCODER_LONG_DOWN	0x08
#define ENCODER_KEY_DOWN	0x10
#define ENCODER_FASTSTEP	0x20
#define ENCODER_LLONG_DOWN	0x40

#define	ENCODER_CW_CCW		0x03

#define ENCODER_NONE 		0x00

#define SHORT_KEY_DOWN_COUNT	100
#define LONG_KEY_DOWN_COUNT		500
#define LLONG_KEY_DOWN_COUNT	1000
#define FASTSTEP_INTERVAL		50

uint8_t rotaryEncoderState(void);

#endif
