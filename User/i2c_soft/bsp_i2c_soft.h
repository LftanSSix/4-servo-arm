#ifndef _BSP_I2C_SOFT_H
#define _BSP_I2C_SOFT_H
#include "./tof/sys.h"

#define PCA_READ_ADDRESS	0x81 //  设备地址40-板1
#define PCA_WRITE_ADDRESS	0x80 //

#define PCA1_READ_ADDRESS	0x83 //  设备地址41-板2
#define PCA1_WRITE_ADDRESS	0x82 //

#define slave0_addr  0x40
#define slave1_addr  0x41

/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define TOF_GPIO_PORT_I2C	GPIOC			/* GPIO端口 */
#define TOF_RCC_I2C_PORT 	RCC_AHB1Periph_GPIOC		/* GPIO端口时钟 */
#define TOF_I2C_SCL_PIN		GPIO_Pin_7			/* 连接到SCL时钟线的GPIO */
#define TOF_I2C_SDA_PIN		GPIO_Pin_6			/* 连接到SDA数据线的GPIO */

#define	SDA_IN()           {GPIOC->MODER&=~(3<<12);GPIOC->MODER|=0<<12;} //PC6输入
#define	SDA_OUT()          {GPIOC->MODER&=~(3<<12);GPIOC->MODER|=1<<12;} //PC6输出

/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
	#define TOF_I2C_SCL_1()  GPIO_SetBits(TOF_GPIO_PORT_I2C, TOF_I2C_SCL_PIN)		/* SCL = 1 */
	#define TOF_I2C_SCL_0()  GPIO_ResetBits(TOF_GPIO_PORT_I2C, TOF_I2C_SCL_PIN)		/* SCL = 0 */
			
	#define TOF_I2C_SDA_1()  GPIO_SetBits(TOF_GPIO_PORT_I2C, TOF_I2C_SDA_PIN)		/* SDA = 1 */
	#define TOF_I2C_SDA_0()  GPIO_ResetBits(TOF_GPIO_PORT_I2C, TOF_I2C_SDA_PIN)		/* SDA = 0 */
			
	#define TOF_I2C_SDA_READ()  GPIO_ReadInputDataBit(TOF_GPIO_PORT_I2C, TOF_I2C_SDA_PIN)	/* 读SDA口线状态 */

void I2C_Configuration(void);
void Data_write(uint8_t addrdata,uint8_t data);
void Data_write_mul(uint8_t mulSlave_addr,uint8_t addrdata,uint8_t data);
void Data_write_mul_N(uint8_t mulSlave_addr,uint8_t addrdata, __IO uint16_t *data,u8 NumOfW);
uint8_t Data_read(uint8_t addrdata);
uint8_t Data_read_mul(uint8_t mulSlave_addr,uint8_t addrdata);
void Data_read_mul_N(uint8_t mulSlave_addr, uint8_t addrdata,uint16_t NumOfR,__IO uint16_t *Read_DATA);

void Data_write_16mul(uint8_t mulSlave_addr,uint16_t addrdata,uint8_t data);
uint8_t Data_read_16mul(uint8_t mulSlave_addr, uint16_t addrdata);

#endif

