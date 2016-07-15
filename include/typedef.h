#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

typedef signed int int32_t;
typedef signed char int8_t;

//typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;

typedef struct{
	int32_t dState;      	// Last position input
  	int32_t iState;      	// Integrator state
  	int8_t  iGain,    	// integral gain
          pGain,    	// proportional gain
          dGain;     	// derivative gain 
} SPid;

#endif