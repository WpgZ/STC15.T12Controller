
#ifndef SDCC_H_20160714
#define SDCC_H_20160714

//Build-in functions

#define _nop_() \
    do{ __asm   nop  __endasm; } while(0)

#endif