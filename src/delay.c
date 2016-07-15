#include "delay.h"

void delay_ms (uint32_t z)//33.1776, 1ms
{
 	uint32_t x, y;
 	for(x=z; x>0; x--){
  		for(y=2550;y>0;y--);
 	}
}
