#ifndef _BSP_I2C_SOFT_H
#define _BSP_I2C_SOFT_H
#include "./tof/sys.h"

#define PCA_READ_ADDRESS	0x81 //  �豸��ַ40-��1
#define PCA_WRITE_ADDRESS	0x80 //

#define PCA1_READ_ADDRESS	0x83 //  �豸��ַ41-��2
#define PCA1_WRITE_ADDRESS	0x82 //

#define slave0_addr  0x40
#define slave1_addr  0x41

/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define TOF_GPIO_PORT_I2C	GPIOC			/* GPIO�˿� */
#define TOF_RCC_I2C_PORT 	RCC_AHB1Periph_GPIOC		/* GPIO�˿�ʱ�� */
#define TOF_I2C_SCL_PIN		GPIO_Pin_7			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define TOF_I2C_SDA_PIN		GPIO_Pin_6			/* ���ӵ�SDA�����ߵ�GPIO */

#define	SDA_IN()           {GPIOC->MODER&=~(3<<12);GPIOC->MODER|=0<<12;} //PC6����
#define	SDA_OUT()          {GPIOC->MODER&=~(3<<12);GPIOC->MODER|=1<<12;} //PC6���

/* �����дSCL��SDA�ĺ꣬�����Ӵ���Ŀ���ֲ�ԺͿ��Ķ��� */
	#define TOF_I2C_SCL_1()  GPIO_SetBits(TOF_GPIO_PORT_I2C, TOF_I2C_SCL_PIN)		/* SCL = 1 */
	#define TOF_I2C_SCL_0()  GPIO_ResetBits(TOF_GPIO_PORT_I2C, TOF_I2C_SCL_PIN)		/* SCL = 0 */
			
	#define TOF_I2C_SDA_1()  GPIO_SetBits(TOF_GPIO_PORT_I2C, TOF_I2C_SDA_PIN)		/* SDA = 1 */
	#define TOF_I2C_SDA_0()  GPIO_ResetBits(TOF_GPIO_PORT_I2C, TOF_I2C_SDA_PIN)		/* SDA = 0 */
			
	#define TOF_I2C_SDA_READ()  GPIO_ReadInputDataBit(TOF_GPIO_PORT_I2C, TOF_I2C_SDA_PIN)	/* ��SDA����״̬ */

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

