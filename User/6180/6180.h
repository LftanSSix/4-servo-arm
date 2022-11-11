#ifndef _6180_H
#define _6180_H
#include "./tof/sys.h"

#define TOF_READ_ADDRESS	0x53 //  …Ë±∏µÿ÷∑0x29
#define TOF_WRITE_ADDRESS	0x52

#define TOF_SLAVE_ADDRESS	0x29 

uint8_t TOF_Init(void);
uint8_t VL6180X_Start_Range(void);
uint8_t VL6180X_Poll_Range(void);
uint8_t VL6180_Read_Range(void);
void VL6180X_Clear_Interrupt(void);

#endif

