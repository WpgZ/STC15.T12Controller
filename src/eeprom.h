#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "typedef.h"


/*Define ISP/IAP/EEPROM command*/ 
#define CMD_IDLE 	0 //Stand-By 
#define CMD_READ 	1//Byte-Read 
#define CMD_PROGRAM 2 //Byte-Program 
#define CMD_ERASE 	3 //Sector-Erase 

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

//void IapIdle(); 
uint8_t IapReadByte(uint32_t addr); 
void IapProgramByte(uint32_t addr, uint8_t dat); 
void IapEraseSector(uint32_t addr);
#endif