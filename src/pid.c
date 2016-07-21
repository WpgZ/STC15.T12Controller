#include "pid.h"
#include "sbit.h"
#include "delay.h"
#include "config.h"
#include "ADC.h"
#include "state.h"

#define PID_MAX_D		1500
#define PID_MIN_D		-1500
#define PID_ERR_UBOUND	500
#define PID_ERR_LBOUND	-500	
#define PWM_TIME 200

uint8_t CODE ntcTable[51] = { 72, 75, 78, 81, 84, 87, 90, 93, 97, 100, 104, 107,
        110, 114, 117, 121, 124, 128, 132, 135, 139, 142, 146, 149, 153, 156,
        160, 163, 167, 170, 173, 177, 180, 183, 186, 189, 192, 195, 198, 201,
        204, 207, 210, 213, 215, 218, 220, 223, 225, 228, 230 };

int32_t GainEx;
int32_t pwmCount;
int32_t heatingLimit;
int32_t RTT;
int32_t PrevRTT;
int32_t ADVref;
int32_t ADVt12;
int32_t iState;
int8_t Tenv;

void PIDInit(void) {
    iState = 0;

    pwmCount = PWM_TIME;
    heatingLimit = PWM_TIME;
    PrevRTT = 0;
}

void CalcADVt12(void) {
    uint8_t idx;
    int32_t dVt12;
    int32_t MIN_ADVt12, Prev_ADVt12;
    uint8_t adcOkTimes = 0;
    if (PWM_TIME - heatingLimit > 50)
        ADVt12 = adc(1);
    else {
        idx = 0;
        MIN_ADVt12 = adc(1);
        Prev_ADVt12 = MIN_ADVt12;
        do {
            ADVt12 = adc(1);
            if (ADVt12 <= MIN_ADVt12) {
                MIN_ADVt12 = ADVt12;
                adcOkTimes = 0;
            } else {
                dVt12 = ADVt12 - Prev_ADVt12;
                if (dVt12 < 2 && dVt12 > 0) {
                    adcOkTimes++;
                    if (adcOkTimes == 3)
                        break;
                } else
                    adcOkTimes = 0;
            }
            Prev_ADVt12 = ADVt12;
        } while (++idx < 50);
    }

    if (ADVt12 > 700) {
        state = STATE_ERROR;
        return;
    }
    ADVt12 -= Vos;
    if (ADVt12 < 0)
        ADVt12 = 0;
}

void CalcTenv(void) {
    int32_t Vntc;
    ADVref = adc(0);
    Vntc = (unsigned long) adc(2) * Vref / ADVref;
    for (Tenv = 0; Tenv < 51; Tenv++) {
        if (((int32_t) ntcTable[Tenv] << 4) >= Vntc)
            break;
    }
    Tenv += Tcomp;
}

void CalcHeatingLimit(void) {
    int32_t pTerm, dTerm, iTerm, error;

    CalcADVt12();
    if (state == STATE_ERROR) {
        heatingLimit = 0;
        return;
    }

    CalcTenv();

    RTT = (unsigned long) ADVt12 * GainEx / ADVref + Tenv * 10;
    error = Tsetting - RTT;

    if (error > PID_ERR_UBOUND) {
        heatingLimit = PWM_TIME;
    } else if (error < PID_ERR_LBOUND)
        heatingLimit = 0;
    else {
        pTerm = (signed long) (error * PID_P) / 100;

        iState += error;
        if (iState < PID_MIN_D)
            iState = PID_MIN_D;
        if (iState > PID_MAX_D)
            iState = PID_MAX_D;
        iTerm = (signed long) (iState * PID_I) / 100;

        dTerm = (signed long) ((PrevRTT - RTT) * PID_D) / 100;

        heatingLimit = pTerm + iTerm + dTerm;
        if (heatingLimit < 0)
            heatingLimit = 0;
        else if (heatingLimit > PWM_TIME)
            heatingLimit = PWM_TIME;
    }

    PrevRTT = RTT;
    LED_RTT = RTT / 10;
}

void T12Switch(void) {
    if (state == STATE_SHUTDOWN || state == STATE_EX_HEATING) {
        return;
    }

    pwmCount++;
    if (T12_SWITCH) {
        if (pwmCount >= heatingLimit) {
            T12_SWITCH = 0;
        }
    }

    if (!T12_SWITCH) {
        if (pwmCount >= PWM_TIME) {
            CalcHeatingLimit();
            pwmCount = 0;
            if (heatingLimit > 0) {
                T12_SWITCH = 1;
            }
        }
    }
}
