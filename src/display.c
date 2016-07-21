#include "display.h"
#include "delay.h"
#include "sbit.h"
#include "state.h"

uint8_t CODE sectors[15] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
        0x7f, 0x6f, 0x73, 0x79, 0x71, 0x50, 0x40 }; //���������ܶ�������0,1,2,3,4,5,6,7,8,9,P,E,F,R,- 
int32_t* led_value;

int8_t flashCount;
BIT ledOn;
uint8_t flashLimit;

uint32_t led_ctrl;
uint8_t digits[3];

void display_digit(uint8_t dig, uint8_t dp) {
    DPY_A = (dig & 0x01);
    DPY_B = (dig & 0x02);
    DPY_C = (dig & 0x04);
    DPY_D = (dig & 0x08);
    DPY_E = (dig & 0x10);
    DPY_F = (dig & 0x20);
    DPY_G = (dig & 0x40);

    DPY_DP = 0;
    if (((led_ctrl & DP_MASK) >> 8) == dp)
        DPY_DP = 1;

    delay_ms(2);
}

void DisplayInit(BIT on, uint32_t ctrl) {
    uint8_t f = ((ctrl & FLASH_BITMASK) >> 4);
    flashCount = 0;
    flashLimit = 0;
    if (f != 0) {
        flashLimit = 100 / f;
        if (!on) {
            flashCount = flashLimit;
            if (led_ctrl & FLASH_OFF_HALF)
                flashCount >>= 2;
        }
    }

    ledOn = on;
    led_ctrl = ctrl;
}

void DisplayCalcDigits(int32_t a) {
    if (state == STATE_SETTING_P) {
        digits[2] = 10;
    } else {
        if (a < 0) {
            a = -a;
            digits[2] = 14;
        } else {
            digits[2] = (a / 100);
            a -= (digits[2] * 100);
        }
    }
    digits[1] = a / 10;
    digits[0] = a - digits[1] * 10;
}

void Display(int32_t a) {
    if (flashLimit) {
        if (ledOn) {
            flashCount++;
            if (flashCount == flashLimit) {
                ledOn = 0;
                if (led_ctrl & FLASH_OFF_HALF)
                    flashCount >>= 2;
            }
        } else {
            flashCount--;
            delay_ms(9);
            if (flashCount == 0)
                ledOn = 1;
            if ((led_ctrl & ALL_ONOFF))
                return;
        }
    }

    if (led_ctrl & CALC_DIGS) {
        DisplayCalcDigits(a);
    }

    if (ledOn || !(led_ctrl & DIG_HUNDRED)) {
        hundreds_place = 0;
        display_digit(sectors[digits[2]], 3);
        hundreds_place = 1;
    }

    if (ledOn || !(led_ctrl & DIG_TEN)) {
        tens_place = 0;
        display_digit(sectors[digits[1]], 2);
        tens_place = 1;
    }

    if (ledOn || !(led_ctrl & DIG_ONE)) {
        ones_place = 0;
        display_digit(sectors[digits[0]], 1);
        ones_place = 1;
    }
}
