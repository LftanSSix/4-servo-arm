/*
	Ӧ��˵����
	�ڷ���I2C�豸ǰ�����ȵ��� i2c_CheckDevice() ���I2C�豸�Ƿ�����

*/
#include "./i2c_soft/bsp_i2c_soft.h"
#include "stm32f4xx.h"
#include "./systick/bsp_SysTick.h"
#include "./tof/sys.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"

// --------- ����I2C�ӿ���ض���-------- //
#define SDA          GPIO_Pin_6
#define	SCL		     GPIO_Pin_7

#define HIGH         1
#define LOW          0

#define SDA_VAL      GPIO_ReadInputDataBit(GPIOC, SDA)
#define SCL_VAL      GPIO_ReadInputDataBit(GPIOC, SCL)

#define I2C_MORE     1
#define I2C_LAST     0
#define I2C_TIMEOUT  255

#define FAILURE      0
#define SUCCEED      1
#define I2C_DELAYTIME 2

static void Sim_I2C_Set(u8 pin, u8 status);
static void Sim_I2C_Stop(void);
static void Sim_I2C_Start(void);
static u8   Sim_I2C_RD_Byte(u8 more);
static u8   Sim_I2C_WR_Byte(u8 data);

void I2C_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(TOF_RCC_I2C_PORT, ENABLE);	/* ��GPIOʱ�� */

	GPIO_InitStructure.GPIO_Pin = TOF_I2C_SCL_PIN | TOF_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //GPIO_Speed_2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TOF_GPIO_PORT_I2C, &GPIO_InitStructure);
}



/*******************************************************************************
������: Sim_I2C_Set
��������: ����I2C
�������:pin:GPIO_Pin_      status:��ƽ״̬
���ز���:NULL
*******************************************************************************/
void Sim_I2C_Set(u8 pin, u8 status)
{
    if(status == HIGH) GPIO_SetBits  (GPIOC, pin);//����
    if(status == LOW)  GPIO_ResetBits(GPIOC, pin);//����
}
/*******************************************************************************
������: Sim_I2C_Start
��������: ��ʼ
�������:NULL
���ز���:NULL
*******************************************************************************/
void Sim_I2C_Start(void)
{
    Sim_I2C_Set(SCL, LOW);  // SCL low
    Sim_I2C_Set(SDA, HIGH); // SDA float, set as output high
    Sim_I2C_Set(SCL, HIGH); // SCL high
    Delay_us(I2C_DELAYTIME);
    Sim_I2C_Set(SDA, LOW);  // SDA high->low while sclk high, S state occur...
    Delay_us(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, LOW);  // SCL low
}
/*******************************************************************************
������: Sim_I2C_Stop
��������: ֹͣ
�������:NULL
���ز���:NULL
*******************************************************************************/
void Sim_I2C_Stop(void)
{
    Sim_I2C_Set(SCL, LOW);  // SCL low
    Sim_I2C_Set(SDA, LOW);  // SDA low
    Delay_us(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, HIGH); // SCL high
    Delay_us(I2C_DELAYTIME);
    Sim_I2C_Set(SDA, HIGH); // SDA low->high while sclk high, P state occur
    Delay_us(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, LOW);  //  SCL low
    Delay_us(I2C_DELAYTIME);
}
/*******************************************************************************
������: Sim_I2C_WR_Byte
��������:��I2Cд��λ����
�������:dataҪд�������
���ز���:SUCCEED:�ɹ�         FAILURE:ʧ��
*******************************************************************************/
u8 Sim_I2C_WR_Byte(u8 data)
{
    u8 i = 8;

    while(i--) { 			        //send out a bit by sda line.
        Sim_I2C_Set(SCL, LOW);                  // sclk low    
        if(data & 0x80) Sim_I2C_Set(SDA, HIGH); // send bit is 1
        else            Sim_I2C_Set(SDA, LOW);  // send bit is 0
        Delay_us(I2C_DELAYTIME);
        Sim_I2C_Set(SCL, HIGH);                 // SCL high
        Delay_us(I2C_DELAYTIME);
        data <<=1;			        // left shift 1 bit, MSB send first.
    }
    Sim_I2C_Set(SCL, LOW);                      // SCL low
    Sim_I2C_Set(SDA, HIGH);                     // SDA set as input
    for(i=I2C_TIMEOUT; i!=0; i--) {		// wait for sda low	to receive ack
        Delay_us(I2C_DELAYTIME);
        if (!SDA_VAL) {
            Sim_I2C_Set(SCL, HIGH);             // SCL high
            Delay_us(I2C_DELAYTIME);
            Sim_I2C_Set(SCL, LOW);              // SCL_LOW();
            Delay_us(I2C_DELAYTIME);
            return SUCCEED;
        }
    }
    return FAILURE;
}
/*******************************************************************************
������: Sim_I2C_RD_Byte
��������:��I2C����λ����
�������:more
���ز���:�����İ�λ����
*******************************************************************************/
u8 Sim_I2C_RD_Byte(u8 more)
{
    u8 i = 8, byte = 0;

    Sim_I2C_Set(SDA, HIGH);             // SDA set as input
    while(i--) {
        Sim_I2C_Set(SCL, LOW);            // SCL low
        Delay_us(I2C_DELAYTIME);
        Sim_I2C_Set(SCL, HIGH);           // SCL high
        Delay_us(I2C_DELAYTIME);
        byte <<=1;				          //recv a bit
        if (SDA_VAL) byte |= 0x01;
    }
    Sim_I2C_Set(SCL, LOW);
    if(!more)	Sim_I2C_Set(SDA, HIGH);   //last byte, send nack.
    else		Sim_I2C_Set(SDA, LOW);    //send ack
    Delay_us(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, HIGH);             // SCL_HIGH();
    Delay_us(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, LOW);
    return byte;
}

void Data_write(uint8_t addrdata,uint8_t data)
{
	Sim_I2C_Start();
	Sim_I2C_WR_Byte(PCA_WRITE_ADDRESS);
	
	Sim_I2C_WR_Byte(addrdata);	
    
	Sim_I2C_WR_Byte(data);
    Sim_I2C_Stop();
}

void Data_write_mul(uint8_t mulSlave_addr,uint8_t addrdata,uint8_t data)
{
	Sim_I2C_Start();
	Sim_I2C_WR_Byte(mulSlave_addr<<1);
	
	Sim_I2C_WR_Byte(addrdata);	
    
	Sim_I2C_WR_Byte(data);
    Sim_I2C_Stop();
}
/*******************************************************************************
������: Data_write_mul_N
��������:��EEPROMд��һ�����ȵ����ݣ�ע�ⳤ��С��256/BLOCK
�������:
	mulSlave_addr:�豸��ַ��ע�ⲻ����дλ
	addrdata     :EEPROM�ڲ���д��ַ��0~255��
	*data        :���������ݻ�����ָ��
	NumOfW       :�����͵������ֽ�����ע������λ16λ
���ز���:�����İ�λ����
*******************************************************************************/
void Data_write_mul_N(uint8_t mulSlave_addr,uint8_t addrdata, __IO uint16_t *data,u8 NumOfW)
{
	u8 i=0,j=0,k=0;
	u8 ave=(u8)(NumOfW)/16; //���ȡ��-����ռ��������ҳ
	u8 ave_yu=NumOfW%16;    //ȡ����-����һҳ���ֽ���
	
	if(ave>=1)              //����һҳ16�ֽ�����
	{
			cycle:
			Sim_I2C_Start();
			Sim_I2C_WR_Byte(mulSlave_addr<<1);
			Sim_I2C_WR_Byte(addrdata);
			for(i=0;i<8;i++) //8��16λ��������һҳ-16�ֽ�/ҳ
			{
				Sim_I2C_WR_Byte((*data)>>8&0xFF);
				Sim_I2C_WR_Byte((*data)&0xFF);
				data++;
			}
			Sim_I2C_Stop();
			addrdata+=16;    //д��mem��ַ����16/ҳ
			ave--;
		    Delay_ms(100);   //��Ҫ����ʱ
			if(ave!=0){goto cycle;}
			
			if(ave_yu!=0)    //����λ
			{
				Sim_I2C_Start();
				Sim_I2C_WR_Byte(mulSlave_addr<<1); 
				Sim_I2C_WR_Byte(addrdata);
				
				for(k=0;k<(ave_yu/2);k++)
				{
					Sim_I2C_WR_Byte((*data)>>8&0xFF);
					Sim_I2C_WR_Byte((*data)&0xFF);
					data++;
				}
				Sim_I2C_Stop();
			}
	}
	else                    //С��һҳ16�ֽ�����
	{
		Sim_I2C_Start();
		Sim_I2C_WR_Byte(mulSlave_addr<<1);
		Sim_I2C_WR_Byte(addrdata);
		for(j=0;j<(NumOfW/2);j++)
		{
//			printf("<16�ֽ�����NumOfW�����ֽ�������%d,j:%d\r\n",NumOfW,j);
//			Delay_ms(2);
			
			Sim_I2C_WR_Byte((*data)>>8&0xFF);
			Sim_I2C_WR_Byte((*data)&0xFF);
			
			data++;
		}
            Sim_I2C_Stop();
	}		
	
}


uint8_t Data_read(uint8_t addrdata)
{
	uint8_t data=0;
	Sim_I2C_Start();
	Sim_I2C_WR_Byte(PCA_WRITE_ADDRESS);

	  Sim_I2C_WR_Byte(addrdata);	
	
	Sim_I2C_Start();
	Sim_I2C_WR_Byte(PCA_READ_ADDRESS);
	data=Sim_I2C_RD_Byte(0);   //Nack
	Sim_I2C_Stop();	
    return data;
}

uint8_t Data_read_mul(uint8_t mulSlave_addr, uint8_t addrdata)
{
	uint8_t data=0;
	Sim_I2C_Start();
	Sim_I2C_WR_Byte(mulSlave_addr<<1);//
//	i2c_WaitAck();

	  Sim_I2C_WR_Byte(addrdata);	
//      i2c_WaitAck(); 	
	
	Sim_I2C_Start();
	Sim_I2C_WR_Byte((mulSlave_addr<<1)|0x01);//
//	i2c_WaitAck();
	data=Sim_I2C_RD_Byte(0);   //Nack
//	i2c_NAck();	
	Sim_I2C_Stop();	
    return data;
}

/*******************************************************************************
������: Data_read_mul_N
��������:��EEPROM����һ�����ȵ����ݣ�ע�ⳤ��С��256/BLOCK
�������:
	mulSlave_addr:�豸��ַ��ע�ⲻ����дλ
	addrdata     :EEPROM�ڲ���д��ַ��0~255��
	NumOfR       :�������������ֽ���
	*DATA        :���������ݻ�����ָ��
���ز���:�����İ�λ����
*******************************************************************************/
 void Data_read_mul_N(uint8_t mulSlave_addr, uint8_t addrdata,uint16_t NumOfR,__IO uint16_t *DATA)
{
	u8 i=0;
		
		Sim_I2C_Start();
		Sim_I2C_WR_Byte(mulSlave_addr<<1);//
			Sim_I2C_WR_Byte(addrdata);	
		Sim_I2C_Start();
		Sim_I2C_WR_Byte((mulSlave_addr<<1)|0x01);//
	
		for(i=0;i<NumOfR;i++)
		{
			DATA[i]=Sim_I2C_RD_Byte(1);   //ack
			
			if(i==(NumOfR-2))
			{
				DATA[(NumOfR-1)]=Sim_I2C_RD_Byte(0);   //nack
				i+=1;
			}
			
		}
		
		Sim_I2C_Stop();	
		
}

  
 void Data_write_16mul(uint8_t mulSlave_addr,uint16_t addrdata,uint8_t data)
{
	Sim_I2C_Start();
	Sim_I2C_WR_Byte(mulSlave_addr<<1);
	
	  Sim_I2C_WR_Byte(addrdata>>8);   //�߰�λ
	  Sim_I2C_WR_Byte(addrdata&0xFF); //�Ͱ�λ
    
	Sim_I2C_WR_Byte(data);
    Sim_I2C_Stop();
}


uint8_t Data_read_16mul(uint8_t mulSlave_addr, uint16_t addrdata)
{
	uint8_t data=0;
	Sim_I2C_Start();
	Sim_I2C_WR_Byte(mulSlave_addr<<1);//

	  Sim_I2C_WR_Byte(addrdata>>8);    //�߰�λ
	  Sim_I2C_WR_Byte(addrdata&0xFF);  //�Ͱ�λ
	
	Sim_I2C_Start();
	Sim_I2C_WR_Byte((mulSlave_addr<<1)|0x01);//
	data=Sim_I2C_RD_Byte(0);   //Nack
	Sim_I2C_Stop();	
    return data;
}
  
  
  
  
  

 
