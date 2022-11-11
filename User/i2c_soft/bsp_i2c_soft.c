/*
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常

*/
#include "./i2c_soft/bsp_i2c_soft.h"
#include "stm32f4xx.h"
#include "./systick/bsp_SysTick.h"
#include "./tof/sys.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"

// --------- 仿真I2C接口相关定义-------- //
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

	RCC_AHB1PeriphClockCmd(TOF_RCC_I2C_PORT, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = TOF_I2C_SCL_PIN | TOF_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //GPIO_Speed_2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TOF_GPIO_PORT_I2C, &GPIO_InitStructure);
}



/*******************************************************************************
函数名: Sim_I2C_Set
函数作用: 配置I2C
输入参数:pin:GPIO_Pin_      status:电平状态
返回参数:NULL
*******************************************************************************/
void Sim_I2C_Set(u8 pin, u8 status)
{
    if(status == HIGH) GPIO_SetBits  (GPIOC, pin);//设置
    if(status == LOW)  GPIO_ResetBits(GPIOC, pin);//重置
}
/*******************************************************************************
函数名: Sim_I2C_Start
函数作用: 开始
输入参数:NULL
返回参数:NULL
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
函数名: Sim_I2C_Stop
函数作用: 停止
输入参数:NULL
返回参数:NULL
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
函数名: Sim_I2C_WR_Byte
函数作用:向I2C写八位数据
输入参数:data要写入的数据
返回参数:SUCCEED:成功         FAILURE:失败
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
函数名: Sim_I2C_RD_Byte
函数作用:向I2C读八位数据
输入参数:more
返回参数:读出的八位数据
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
函数名: Data_write_mul_N
函数作用:向EEPROM写入一定长度的数据，注意长度小于256/BLOCK
输入参数:
	mulSlave_addr:设备地址，注意不带读写位
	addrdata     :EEPROM内部读写地址（0~255）
	*data        :待发送数据缓冲区指针
	NumOfW       :待发送的数据字节数，注意数据位16位
返回参数:读出的八位数据
*******************************************************************************/
void Data_write_mul_N(uint8_t mulSlave_addr,uint8_t addrdata, __IO uint16_t *data,u8 NumOfW)
{
	u8 i=0,j=0,k=0;
	u8 ave=(u8)(NumOfW)/16; //相除取整-数据占几个整数页
	u8 ave_yu=NumOfW%16;    //取余数-不足一页的字节数
	
	if(ave>=1)              //大于一页16字节数据
	{
			cycle:
			Sim_I2C_Start();
			Sim_I2C_WR_Byte(mulSlave_addr<<1);
			Sim_I2C_WR_Byte(addrdata);
			for(i=0;i<8;i++) //8个16位数据填满一页-16字节/页
			{
				Sim_I2C_WR_Byte((*data)>>8&0xFF);
				Sim_I2C_WR_Byte((*data)&0xFF);
				data++;
			}
			Sim_I2C_Stop();
			addrdata+=16;    //写入mem地址增加16/页
			ave--;
		    Delay_ms(100);   //必要的延时
			if(ave!=0){goto cycle;}
			
			if(ave_yu!=0)    //余数位
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
	else                    //小于一页16字节数据
	{
		Sim_I2C_Start();
		Sim_I2C_WR_Byte(mulSlave_addr<<1);
		Sim_I2C_WR_Byte(addrdata);
		for(j=0;j<(NumOfW/2);j++)
		{
//			printf("<16字节数据NumOfW（总字节数）：%d,j:%d\r\n",NumOfW,j);
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
函数名: Data_read_mul_N
函数作用:从EEPROM读出一定长度的数据，注意长度小于256/BLOCK
输入参数:
	mulSlave_addr:设备地址，注意不带读写位
	addrdata     :EEPROM内部读写地址（0~255）
	NumOfR       :待结束的数据字节数
	*DATA        :待接收数据缓冲区指针
返回参数:读出的八位数据
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
	
	  Sim_I2C_WR_Byte(addrdata>>8);   //高八位
	  Sim_I2C_WR_Byte(addrdata&0xFF); //低八位
    
	Sim_I2C_WR_Byte(data);
    Sim_I2C_Stop();
}


uint8_t Data_read_16mul(uint8_t mulSlave_addr, uint16_t addrdata)
{
	uint8_t data=0;
	Sim_I2C_Start();
	Sim_I2C_WR_Byte(mulSlave_addr<<1);//

	  Sim_I2C_WR_Byte(addrdata>>8);    //高八位
	  Sim_I2C_WR_Byte(addrdata&0xFF);  //低八位
	
	Sim_I2C_Start();
	Sim_I2C_WR_Byte((mulSlave_addr<<1)|0x01);//
	data=Sim_I2C_RD_Byte(0);   //Nack
	Sim_I2C_Stop();	
    return data;
}
  
  
  
  
  

 
