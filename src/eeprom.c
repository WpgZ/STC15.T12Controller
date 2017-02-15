#include "eeprom.h"

#include <mach/mcu51.h>


/*Define ISP/IAP/EEPROM operation const for IAP_CONTR*/
//#define ENABLE_IAP 0x80 //if SYSCLK<30MHz 
#define ENABLE_IAP 	0x81 //if SYSCLK<24MHz 
//#define ENABLE_IAP 0x82 //if SYSCLK<20MHz 
//#define ENABLE_IAP 0x83 //if SYSCLK< 2MHz 
//#define ENABLE_IAP 0x84 //if SYSCLK<6MHz 
//#define ENABLE_IAP 0x85 //if SYSCLK<3MHz 
//#define ENABLE_IAP 0x86 //if SYSCLK<2MHz 
//#define ENABLE_IAP 0x87 //if SYSCLK< MHz 

//Start address for STC 5F204EA series EEPROM 
#define IAP_ADDRESS 0x0000

/*Define ISP/IAP/EEPROM command*/
enum{
    CMD_IDLE,
    CMD_READ,
    CMD_PROGRAM,
    CMD_ERASE
};

/*---------------------------Disable 
 ISP/IAP/EEPROM function 
 Make MCU in a safe state 
 see page 233 in STC15F204EA-series-chinese[1]: ;?�������ɲ���?ֻ�ǳ��ڰ�ȫ���Ƕ���
 ----------------------------*/
/*void IapIdle() 
 {
    IAP_CONTR = 0; //Close IAP function 
    IAP_CMD = 0; //Clear command to standby 
    IAP_TRIG = 0; //Clear trigger register 
    IAP_ADDRH = 0x80; //Data ptr point to non-EEPROM area 
    IAP_ADDRL = 0; //Clear IAP address to prevent misuse
 } 
 */

static void IapOperation(uint32_t addr, uint8_t dat, uint8_t cmd) {
    IAP_CONTR = ENABLE_IAP; //Open IAP function, and set wait time 
    IAP_CMD = cmd; //Set ISP/IAP/EEPROM PROGRAM command 
    IAP_ADDRL = addr; //Set ISP/IAP/EEPROM address low 
    IAP_ADDRH = addr >> 8; //Set ISP/IAP/EEPROM address high 
    if (cmd == CMD_PROGRAM)
        IAP_DATA = dat; //Write ISP/IAP/EEPROM data 
    IAP_TRIG = 0x5a; //Send trigger command (0x5a) 
    IAP_TRIG = 0xa5; //Send trigger command2 (0xa5) 
    _nop_(); //MCU will hold here until ISP/IAP/EEPROM operation complete 
}

void IapEraseSector(uint32_t addr) {
    IapOperation(addr, 0, CMD_ERASE);
}

uint8_t IapReadByte(uint32_t addr) {
    IapOperation(addr, 0, CMD_READ);
    return IAP_DATA;
}

void IapProgramByte(uint32_t addr, uint8_t val) {
    IapOperation(addr, val, CMD_PROGRAM);
}
