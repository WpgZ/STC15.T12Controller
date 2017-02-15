#include "config.h"

#include "rotaryencoder.h"
#include "display.h"
#include "state.h"
#include "eeprom.h"
#include "sbit.h"
// #include "beep.h"
#include "ADC.h"

int32_t LED_RTT;
uint8_t state = STATE_HEATING;
int32_t Tsetting;
int8_t Tcomp;
uint8_t PID_P, PID_I, PID_D;
uint8_t Vos;
int32_t Vref;
BIT PRE_HIBER_STATE;
uint8_t next_state;
int32_t lastParameter = S_TEMP_T12;

uint8_t old_state = STATE_NONE;
int32_t exHeatingTime;
uint8_t passEclipsed = 0;
BIT passVerified = 0;
uint8_t led_idx;

int32_t IDATA Setting[16] = { 200, S_EMPTY_PASS, 249, 64, 4, 16, 41, 0, 0, 200,
        S_UNDEFINED, S_UNDEFINED, S_UNDEFINED, S_FREE, S_UNDEFINED, 0 };
uint8_t hiberEclipsed;

int32_t exHeatEclipsed_1S = 1000;
int32_t exHeatEclipsed;

int32_t noInputEclipsed = 0;

uint32_t sec60Count = 60000;
int32_t noInputCount = 0;
uint8_t minCount = 0;

int32_t calcRotateValue(int32_t value, BIT cw, int32_t min, int32_t max,
        BIT loop, uint8_t step) {
    int32_t ret = value;
    if (cw) {
        ret += step;
        if (ret > max)
            ret = loop ? min : max;
    } else {
        ret -= step;
        if (ret < min)
            ret = loop ? max : min;
    }
    return ret;
}

int32_t calcSettingValue(int32_t val, BIT cw, BIT fs, uint8_t parameter) {
    int32_t min = 0;
    int32_t max = 255;
    BIT loop = 1;
    uint8_t step = fs ? 10 : 1;
    switch (parameter) {
    case S_TEMP_T12:
        min = 200;
        max = 480;
        break;
        //case S_PASS: 	handled in STATE_PASS_CONFIG
    case S_VREF:
        min = 1;
        max = 500;
        break;
    case S_EDT:
        min = 1;
        break;
    case S_TEMP_HIBER:
        min = 200;
        break;
    case S_TEMP_ADJUST:
        max = 2;
        break;
    case S_SAVE_LOAD:
        max = 1;
        break;
    case S_TEMP_COMP:
        max = 30;
        min = -30;
        loop = 0;
        break;
    default:
        //S_VOS
        //S_PASS_TIMEOUT
        //S_PID_P
        //S_PID_I
        //S_PID_D
        //S_NOINPUT_TIMEOUT
        //S_TIME_HIBER
        //S_TIME_SHUTDOWN
        //S_TIME_EXHEATING
        break;
    }
    return calcRotateValue(val, cw, min, max, loop, step);
}

BIT verifyPass(uint8_t ns) {
    if (Setting[S_PASS] != S_EMPTY_PASS && !passVerified) {
        state = STATE_PASS_VERIFY;
        next_state = ns;
        DisplayCalcDigits(S_EMPTY_PASS);
        return 0;
    }
    return 1;
}

void ReadConfigs(void) {
    uint32_t addr = 0x0000;
    uint8_t idx = 0;
    if (IapReadByte(addr++) != SETTING_NUM)
        return;

    do {
        Setting[idx] = ((IapReadByte(addr++) << 8) | (IapReadByte(addr++)));
    } while (++idx != SETTING_NUM - 1);
}

void SaveConfigs(void) {
    uint32_t addr = 0x0000;
    uint8_t idx = 0;
    IapEraseSector(0);
    IapProgramByte(addr++, SETTING_NUM);

    do {
        IapProgramByte(addr++, Setting[idx] >> 8);
        IapProgramByte(addr++, Setting[idx] & 0x00FF);
    } while (++idx != SETTING_NUM - 1);
}

void onEncoderRotate(BIT cw, BIT fs) {
    switch (state) {
    case STATE_SETTING_P:
        lastParameter = calcRotateValue(lastParameter, cw, 0, SETTING_NUM - 1,
                1, fs ? 3 : 1);
        break;
    case STATE_HEATING: {
        switch (Setting[S_TEMP_ADJUST]) {
        case S_PROTECTED:
            if (noInputCount == 0 && !verifyPass(STATE_HEATING))
                return;
        case S_FIXED:
            if (noInputCount == 0) {
                state = STATE_HIBERNATE;
                noInputCount = 2000;
            }
            break;
        default:
            //case S_FREE:
            lastParameter = S_TEMP_T12;
            state = STATE_SETTING_V;
            noInputEclipsed = 2000;		//Setting1[S1_NOINPUT_TIMEOUT];
            break;
        }
    }
        break;
    case STATE_SETTING_V:
        Setting[lastParameter] = calcSettingValue(Setting[lastParameter], cw,
                fs, lastParameter);
        break;
    case STATE_EX_PREHEATING:
        exHeatingTime = calcSettingValue(exHeatingTime, cw, fs,
                S_TIME_EXHEATING);
        break;
    case STATE_PASS_CONFIG:
    case STATE_PASS_VERIFY:
        digits[led_idx] = calcRotateValue(digits[led_idx], cw, 0, 9, 1, 1);
        if (state == STATE_PASS_VERIFY
                && (digits[2] * 100 + digits[1] * 10 + digits[0])
                        == Setting[S_PASS]) {
            state = next_state;
            passEclipsed = Setting[S_PASS_TIMEOUT];
            passVerified = 1;
        }

        break;
    default:
        if (noInputCount == 0) {
            state = STATE_HEATING;
            noInputCount = 2000;
        }
        break;
    }
}

void onEncoderLongPressed(void) {
    switch (state) {
    case STATE_HEATING:
    case STATE_HIBERNATE:
        if (!verifyPass(STATE_SETTING_P))
            return;
        state = STATE_SETTING_P;
        break;
    case STATE_SETTING_P:
        state = STATE_HEATING;
        break;
    case STATE_PASS_CONFIG:
    case STATE_SETTING_V:
        state = STATE_SETTING_P;
        break;
    default:
        state = STATE_HEATING;
        break;
    }
}

void onEncoderShortPressed(void) {
    uint32_t ctrl;
    switch (state) {
    case STATE_SETTING_P:
        if (lastParameter == S_PASS) {
            state = STATE_PASS_CONFIG;
            DisplayCalcDigits(Setting[S_PASS]);
        } else {
            state = STATE_SETTING_V;
            //updateLED_S();
        }
        break;
    case STATE_SETTING_V:
        state = STATE_SETTING_P;
        break;
    case STATE_HEATING:
        state = STATE_HIBERNATE;
        break;
    case STATE_PASS_CONFIG:
    case STATE_PASS_VERIFY:
        if (led_idx != 0)
            led_idx--;
        else
            led_idx = 2;
        ctrl = (led_ctrl & 0xFFF8);
        ctrl |= (1 << led_idx);
        DisplayInit(0, ctrl);
        break;
    default:
        state = STATE_HEATING;
        break;
    }
}

void onEncoderLLongPressed(void) {
    BIT buzz = 1;
    switch (state) {
    case STATE_HEATING:
    case STATE_HIBERNATE:
        state = STATE_EX_PREHEATING;
        break;
    case STATE_EX_PREHEATING:
        state = STATE_EX_HEATING;
        T12_SWITCH = 1;	 	 //ugly, just for reducing the hex size
        break;
    case STATE_PASS_CONFIG:
    case STATE_SETTING_V:
        switch (lastParameter) {
        case S_PASS:
            Setting[S_PASS] = digits[2] * 100 + digits[1] * 10 + digits[0];
            break;
        case S_SAVE_LOAD:
            if (Setting[S_SAVE_LOAD] == 1)
                SaveConfigs();
            else
                ReadConfigs();
            break;
        case S_VOS:
            if (Setting[S_VOS] == 0)
                Setting[S_VOS] = adc(1);
            break;
        default:
            buzz = 0;
        }
        //BeepInit(BEEP_ONCE|BEEP_SHORT);
        BUZZ = buzz;
        state = STATE_SETTING_P;
        break;
    }
}

void refreshState(void) {
    BIT on = 1;
    uint32_t ctrl = CTRL_DEF;
    if (old_state != state) {
        switch (state) {
        case STATE_HIBERNATE:
            on = 0;
            ctrl |= FLASH_1000;
            hiberEclipsed = Setting[S_TIME_SHUTDOWN];
            Tsetting = Setting[S_TEMP_HIBER] * 10 + 5;
            led_value = &LED_RTT;
            break;
        case STATE_SHUTDOWN:
            ctrl &= ~CALC_DIGS;
            digits[2] = digits[1] = digits[0] = 14;
            break;
        case STATE_EX_PREHEATING:
            led_value = &exHeatingTime;
            ctrl |= FLASH_500 | FLASH_OFF_HALF;
            break;
        case STATE_PASS_VERIFY:
        case STATE_PASS_CONFIG:
            led_idx = 2;
            ctrl = DIG_HUNDRED | FLASH_500 | FLASH_OFF_HALF;
            break;
        case STATE_SETTING_V:
            if (lastParameter == S_VREF)
                ctrl |= DP_3;
            led_value = &Setting[lastParameter];
            break;
        case STATE_SETTING_P:
            led_value = &lastParameter;
            break;
        case STATE_EX_HEATING:
            exHeatEclipsed = exHeatingTime;
            led_value = &exHeatEclipsed;
            break;
        case STATE_ERROR:
            ctrl &= ~CALC_DIGS;
            digits[2] = 11;
            digits[1] = digits[0] = 13;
            //BeepInit(BEEP_SHORT);
            BUZZ = 1;
            break;
        default:
            //case STATE_HEATING:
            Tsetting = Setting[S_TEMP_T12] * 10 + 5;
            Tcomp = Setting[S_TEMP_COMP];
            Vos = Setting[S_VOS];
            Vref = Setting[S_VREF] * 10;
            GainEx = (unsigned long) Vref * Setting[S_EDT] / 27;
            PID_P = Setting[S_PID_P];
            PID_I = Setting[S_PID_I];
            PID_D = Setting[S_PID_D];

            hiberEclipsed = Setting[S_TIME_HIBER];
            exHeatEclipsed = 0;
            led_value = &LED_RTT;
            break;
        }
        old_state = state;
        DisplayInit(on, ctrl);
    }
}

void DoConfig(void) {
    uint8_t encoderState;
    if (PRE_HIBER_STATE != HIBER_STATE) {
        if (state == STATE_HIBERNATE || state == STATE_SHUTDOWN)
            state = STATE_HEATING;
        PRE_HIBER_STATE = HIBER_STATE;
        hiberEclipsed = Setting[S_TIME_HIBER];
    }

    if (state == STATE_EX_HEATING) {
        exHeatEclipsed_1S--;
        if (exHeatEclipsed_1S == 0) {
            --exHeatEclipsed;
            if (exHeatEclipsed == 0)
                state = STATE_HEATING;
            exHeatEclipsed_1S = 1000;
        }
    }

    if (noInputEclipsed != 0) {
        noInputEclipsed--;
        if (noInputEclipsed == 0)
            state = STATE_HEATING;
    }

    if (noInputCount != 0)
        noInputCount--;

    sec60Count--;
    if (sec60Count == 0) {
        if (passEclipsed != 0)
            passVerified = --passEclipsed;

        if (hiberEclipsed != 0) {
            hiberEclipsed--;
            if (hiberEclipsed == 0) {
                if (state == STATE_HEATING)
                    state = STATE_HIBERNATE;
                else {	 	 // STATE_HIBERNATE:
                    state = STATE_SHUTDOWN;
                    T12_SWITCH = 0;	 //ugly, just for reducing the hex size
                }
            }
        }
        sec60Count = 60000;
    }

    encoderState = rotaryEncoderState();
    if (encoderState != ENCODER_NONE) {
        if (encoderState & ENCODER_CW_CCW)
            onEncoderRotate((encoderState & ENCODER_CW),
                    encoderState & ENCODER_FASTSTEP);
        else if (encoderState == ENCODER_SHORT_DOWN)
            onEncoderShortPressed();
        else if (encoderState == ENCODER_LONG_DOWN)
            onEncoderLongPressed();
        else
            onEncoderLLongPressed();

        if (state >= STATE_CONFIG_START && Setting[S_NOINPUT_TIMEOUT] != 0)
            noInputEclipsed = Setting[S_NOINPUT_TIMEOUT] * 1000;
    }
}

void Config(void) {
    if (state != STATE_ERROR)
        DoConfig();

    refreshState();
}
