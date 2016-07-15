#include "sbit.h"
#include "display.h"
#include "pid.h"
#include "config.h"
#include "state.h"
#include "beep.h"
#include "eeprom.h"

void main (void)
{
	BUZZ	= 0; 
	P1M0	= 0x80;
 	P1ASF	= 0x07;
	P2M0	= 0xFF;
	P3M1	= 0x0C;
 	P3M0	= 0xF0;

	TMOD	= 0x00; 	
	AUXR	= 0xC0; //timer0 , timer1 1T 	 

 	ADC_CONTR = 0x80;
	 	
	if(!ENCODER_D)
		IapEraseSector(0x0000);// password will be useless 

	ReadConfigs();//Warning L15 will arise, disable it in Tab "BL51 Misc"
	PRE_HIBER_STATE = !HIBER_STATE;
		 
	PIDInit();

	TL1 = 0x66;		//timer1, 33.1776, 1ms
    TH1 = 0x7E;
 	ET1  = 1;
 	TR1  = 1;

	EA = 1;
 	while(1){	
		Display(*led_value);
		Beep();
 	}
}

void Timer1(void) INTERRUPT 3
{
	Config();
	T12Switch();
}