#include "ADC.h"

#include <mach/mcu51.h>

int32_t adc(uint8_t ch) {
    ADC_CONTR = 0x88 | ch; //|ADC_SPEEDH;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & 0x10))
        ;
    ADC_CONTR &= ~0x10;
    return (ADC_RES << 2) | ADC_RESL;
}
