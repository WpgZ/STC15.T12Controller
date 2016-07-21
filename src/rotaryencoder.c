/*
 most part of this file comes from goodcode, bbs.yleee.com.cn
 */

#include "rotaryencoder.h"
#include "sbit.h"

BIT prevEncoderA = 1;
BIT prevEncoderB = 1;
BIT prevEncoderD = 1;

uint32_t keydownCount = 0;
uint8_t intervalCount = 0;

uint8_t fastStep() {
    uint8_t result = 0;
    if (intervalCount != 0 && intervalCount < FASTSTEP_INTERVAL)
        result = ENCODER_FASTSTEP;

    intervalCount = 1;
    return result;
}

uint8_t rotaryState(void) {
    uint8_t result = ENCODER_NONE;
    if (intervalCount && intervalCount != FASTSTEP_INTERVAL)
        intervalCount++;

    if (prevEncoderA) {
        if (!ENCODER_A)
            prevEncoderB = ENCODER_B;
    } else {
        if (ENCODER_A) {
            if (ENCODER_B) {
                if (!prevEncoderB)
                    result = (ENCODER_CW | fastStep());

            } else {
                if (prevEncoderB)
                    result = (ENCODER_CCW | fastStep());
            }

        }
    }

    prevEncoderA = ENCODER_A;
    return result;
}

uint8_t keydownState(void) {
    uint8_t result = ENCODER_NONE;
    if (ENCODER_D) {
        if (!prevEncoderD) {
            if (keydownCount >= LONG_KEY_DOWN_COUNT)
                result = ENCODER_LONG_DOWN;
            else if (keydownCount != 0)
                result = ENCODER_SHORT_DOWN;
        }
        keydownCount = 0;
    } else {
        if (prevEncoderD)
            keydownCount = 1;
        else if (keydownCount != 0) {
            keydownCount++;
            if (keydownCount > LLONG_KEY_DOWN_COUNT) {
                result = ENCODER_LLONG_DOWN;
                keydownCount = 0;
            }
        }

    }
    prevEncoderD = ENCODER_D;
    return result;
}

uint8_t rotaryEncoderState(void) {
    uint8_t result = rotaryState();
    if (result != ENCODER_NONE)
        return result;

    return keydownState();
}

