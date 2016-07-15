#ifndef __STATE_H__
#define __STATE_H__
#include "typedef.h"

enum {
    STATE_ERROR			= 0x00,
    STATE_POWERON		= 0x01,
    STATE_HEATING		= 0x02,
    STATE_HIBERNATE		= 0x03,
    STATE_SHUTDOWN		= 0x04,
    STATE_EX_HEATING	= 0x05,
    STATE_SETTING_P     = 0x06,
    STATE_SETTING_V	    = 0x07,
    STATE_EX_PREHEATING	= 0x08,
    STATE_PASS_VERIFY	= 0x09,
    STATE_PASS_CONFIG	= 0x0A,

    STATE_NONE			= 0xFE
};

#define STATE_CONFIG_START	STATE_SETTING_P	

extern uint8_t state;

#endif 