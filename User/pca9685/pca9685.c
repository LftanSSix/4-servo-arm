/*
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常

*/

#include "./pca9685/pca9685.h"
#include "./i2c_soft/bsp_i2c_soft.h"
#include "stm32f4xx.h"
#include "./systick/bsp_SysTick.h"
#include "./tof/sys.h"
#include <math.h>

/*设置板1周期*/
void setPWMFreq(u8 freq)  
{
   u8 prescale,oldmode,newmode;
   double prescaleval;
	
   prescaleval = 25000000.0/(4096*freq*0.915);
   prescale = (u8)floor(prescaleval+0.5)-1;

   oldmode = Data_read(PCA9685_MODE1);
   newmode = (oldmode&0x7F) | 0x10; // sleep
   Data_write(PCA9685_MODE1, newmode); // go to sleep
   Data_write(PCA9685_PRESCALE, prescale); // set the prescaler
   Data_write(PCA9685_MODE1, oldmode);
   Delay_ms(5);
   Data_write(PCA9685_MODE1, 0xa1); 

}

/*设置板2周期*/
void setPWMFreq1(u16 freq)  
{
   u8 prescale,oldmode,newmode;
   double prescaleval;
	
   prescaleval = 25000000.0/(4096*freq*0.915);
   prescale = (u8)floor(prescaleval+0.5)-1;

   oldmode = Data_read_mul(slave1_addr,PCA9685_MODE1);
   newmode = (oldmode&0x7F) | 0x10; // sleep
   Data_write_mul(slave1_addr,PCA9685_MODE1, newmode); // go to sleep
   Data_write_mul(slave1_addr,PCA9685_PRESCALE, prescale); // set the prescaler
   Data_write_mul(slave1_addr,PCA9685_MODE1, oldmode);
   Delay_ms(5);
   Data_write_mul(slave1_addr,PCA9685_MODE1, 0xa1); 

}

/*
   参数：
         
*/
void setPWM(u8 num, u16 on, u16 off) 
{
    Data_write(LED0_ON_L+4*num,on);//0 0  102
    Data_write(LED0_ON_H+4*num,on>>8);
    Data_write(LED0_OFF_L+4*num,off);
    Data_write(LED0_OFF_H+4*num,off>>8);
}  

void setPWM1(u8 num, u16 on, u16 off) 
{
    Data_write_mul(slave1_addr,LED0_ON_L+4*num,on&0xFF);//0 0  102
    Data_write_mul(slave1_addr,LED0_ON_H+4*num,on>>8);
    Data_write_mul(slave1_addr,LED0_OFF_L+4*num,off&0xFF);
    Data_write_mul(slave1_addr,LED0_OFF_H+4*num,off>>8);
	
//    Data_write_mul(slave1_addr,LED0_ON_L+4*num,on);//0 0  102
//    Data_write_mul(slave1_addr,LED0_ON_H+4*num,(on>>8));
//    Data_write_mul(slave1_addr,LED0_OFF_L+4*num,off);
//    Data_write_mul(slave1_addr,LED0_OFF_H+4*num,(off>>8));
}  

/*角度-off值转换50hz 180度*/
u16 calculate_PWM(u8 angle){
    return (int)(204.8*(0.5+angle*1/90));
}  

/*角度-off值转换300hz 270度*/
u16 calculate_PWM1(u16 angle){
    return (int)floor(204.8*(0.5+angle*1/(90*1.5)));
}  

/*
   参数：servo_x: 
				0-180度舵机
				1-270度舵机
*/
void set_angle(u8 num,u8 servo_x,u16 angle)
{
	u16 a,b;
	a=calculate_PWM(angle);
	b=calculate_PWM1(angle);
	switch(servo_x)
	{
		case 0:
			setPWM(num, 0, a);
			break;
		case 1:
			setPWM1(num, 0, b);
			break;
		
	}
}






 
