#ifndef _PCA9685_H_
#define _PCA9685_H_
#include "./tof/sys.h"

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

void setPWMFreq(u8 freq);
void setPWMFreq1(u16 freq);
void setPWM(u8 num, u16 on, u16 off);
void setPWM1(u8 num, u16 on, u16 off); 
u16 calculate_PWM(u8 angle);
u16 calculate_PWM1(u16 angle);
void set_angle(u8 num,u8 servo_x,u16 angle) ;

#endif

