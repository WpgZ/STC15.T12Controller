#include "beeper.h"
#include "sbit.h"

static int8_t beepCount = 20;

void Beep(void) {
    if (BUZZ) {
        beepCount--;
        if (beepCount == 0) {
            beepCount = 20;
            BUZZ = 0;
        }
    }
}
