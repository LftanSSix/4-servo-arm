/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief  OLED����
//		LED_BLUE;     ��ʱ����
//		Delay_us(1000000);
//		LED_RED;
//		Delay_s(5);
//		LED_YELLOW;
//		Delay_ms(1000);
		VL6180X_Start_Range();
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./tof/sys.h"
#include "./i2c_soft/bsp_i2c_soft.h"
#include "./pca9685/pca9685.h"
#include "./6180/6180.h"
#include "./adc/bsp_adc.h"
#include "./key/bsp_exti.h"
#include "./tim/bsp_basic_tim.h"

#define del_time 2
#define len_data 0xFC //��������(8λ)���ȴ�ŵ�ַ
extern __IO uint16_t ADC_ConvertedValue[NumberOfChannel];
extern __IO u16 aver_value[4];
extern u8 KEY_flag,KEY1_flag;

__IO u16 send[256];
__IO u16 recive[256];
u16 Read_DATA[1024];
//u8 distance;
//float temp_angle[NumberOfChannel]={0}; 
u8 j,read_value;

int main(void)
{
	static u8 t=0,counter=0; //ÿ����ɷ�126��������63ҳ��+1ҳlen
	static u16 data_sum_Num;
	u16 trans[256];
	EXTI_Key_Config();
	TIMx_Configuration();
	SysTick_Init();   //��ʼ���ӳٺ���
   	LED_GPIO_Config();
	Debug_USART_Config();
	Rheostat_Init();
	LED_BLUE;
	I2C_Configuration();
//	TOF_Init();   //tof050��ʼ������
    Data_write(PCA9685_MODE1,0x0); //д��ַ
	Delay_ms(2);
	Data_write_mul(slave1_addr,PCA9685_MODE1,0x0);
	setPWMFreq(54);
//	Delay_ms(2);
//	setPWMFreq1(52);
	
#if(test==0) //����������

#else
//test
u16 sendi[20]={0x069,0x0D5,0x120,0x11E,0x187,0x0D4,0x120,0x121,0x148,0x0D5,0x122,0x120,0x0B1,0x0D4,0x120,0x119,0x184,0x0D6,0x121,0x116} ;
u16 sendj[16]={0x069,0x0D5,0x120,0x11E,0x187,0x0D4,0x120,0x121} ;
u16 sendk[40]={00,105,00,213,01,32,01,30,01,135,00,212,01,32,01,33,01,72,00,213,01,34,01,32,00,177,00,212,01,32,01,25,01,132,00,214,01,33,01,22} ;
u16 sendk1[20]={105,213,288,286,391,212,288,289,328,213,290,288,177,212,288,281,388,214,289,278} ;
//105,213,288,286,391,212,288,289,328,213,290,288,177,212,288,281,388,214,289,278		
for(i=0;i<12;i++)
{
	send[i]=sendi[i];
	printf("sendi%d; %d\r\n",i,send[i]);
	Delay_ms(2);
}
//for(i=0;i<8;i++)//С��16
//{
//	send[i]=sendj[i];
//	printf("sendi%d: %d\r\n",i,send[i]);
//	Delay_ms(2);
//}
#endif
	
	while(1)
	{
#if(1)	//������	
		get_aver_value();
		
		setPWM(0,0,aver_value[0]);
		Delay_ms(del_time);
		
		setPWM(1,0,aver_value[1]);
		Delay_ms(del_time);
		
		setPWM(2,0,aver_value[2]);
		Delay_ms(del_time);
		
		setPWM(3,0,aver_value[3]);
		Delay_ms(del_time);
		
//		printf("%d-%d-%d-%d\r\n",aver_value[0],aver_value[1],aver_value[2],aver_value[3]);
		
/********������������ݷ���һ�����飬ÿ�����ݴ�СΪ16λ****************/		
		if(KEY_flag==1)
		{
			counter++;//ÿ�̰�һ�ν���λ�ü��䣬counter��ΪNumOfW= counter*4
			u8 times=0,i97=0,i108=0;
			while(times!=4)
			{
				send[t] = aver_value[i97];
				t++;
				i97++;
				times++;
				LED_RED;
				Delay_ms(300);  //��Ҫ����ʱ
				LED_GREEN;
			}
				Delay_ms(10);
			for(i108=0;i108<20;i108++)
			{
				printf("д�뻺��ֵ%d��%d\r\n",i108,send[i108]);
				Delay_ms(30);
			}
			KEY_flag=0;	
		} 
/******************�����������ݳ�ȥ*****************************/	
#if(test==0) //������

#else
		#define l5 3
		if(KEY_flag==2)
		{
			Data_write_mul_N(0x50,0x00,send,8*l5);  //8*counter:Ҫ���͵����ֽ���
			Delay_ms(20);
			
			LED_YELLOW;
			
				Delay_ms(500);
				KEY_flag=0;
		}
#endif
		
		if(KEY_flag==2)
		{
			Data_write_mul_N(0x50,0x00,send,8*counter);  //8*counter:Ҫ���͵����ֽ���
			Delay_ms(20);
			Data_write_mul(0x50,len_data,8*counter);//�����ܳ�����Bytes
			printf("NumOfW��%d\r\n",8*counter);
			
			LED_YELLOW;
			
			u8 i=0;
			for(i=0;i<(4*counter);i++)
			{
				printf("д�����%d��%d\t",i,send[i]);
				Delay_ms(20);
			}	
				printf("\r\n");
				Delay_ms(500);
				KEY_flag=0;
		}

/******************��ȡ������EEPROM�����ݣ���������λ��ÿ�����ݴ�СΪ8λ*****************************/
	while(KEY1_flag==1) //�̰�������
	{
		u8 i,j=0,b1,b2;;
		data_sum_Num=Data_read_mul(0x50,len_data);
		printf("leng_data:%d\r\n",data_sum_Num);
		
		Data_read_mul_N(0x50,0x00,data_sum_Num,recive);
		printf("��ȡԭʼ���ݣ�\r\n");
		
		for(i=0;i<data_sum_Num;i++)
		{
			printf("%d-%d\t",i,recive[i]);
		}
		    printf("\r\n");
		
		    LED_WHITE;
		
		for(i=0;i<(data_sum_Num-1);) //40-1
		{
			b1=recive[i];
			b2=recive[i+1];
			i+=2;
			trans[j]=(u16)((b1&0xFF)<<8|(b2&0xFF));
			printf("trans[%d]:%d\r\n",j,trans[j]);
			j++;
			Delay_ms(2);
		}
		
			KEY1_flag=0;
	}

#if(test==0) //������

#else
	#define a7 80
	while(KEY1_flag==1)
	{
		u8 i143;
		Delay_ms(50);
		
		Data_read_mul_N(0x50,0x00,a7,recive);
		printf("��ȡԭʼ���ݣ�\r\n");
		
		for(i143=0;i143<a7;i143++)
		{
			printf("%d-%d\t",i143,recive[i143]);
		}
		    printf("\r\n");
		
			KEY1_flag=0;
	}
#endif

/******************��ȡ�����е�pwm����*****************************/
	#define t1 500
	u8 i=0,j=0;
	while(KEY1_flag==2)  //�������Ŷ���
	{
		printf("��������%d\r\n",data_sum_Num);
		setPWM(0,0,trans[i]);
		printf("%d\r\n",trans[i]);
		Delay_ms(t1);
		LED_RED;
		setPWM(1,0,trans[i+1]);
	  	Delay_ms(t1);
	  	
	  	setPWM(2,0,trans[i+2]);
	  	Delay_ms(t1);
	  	
	  	setPWM(3,0,trans[i+3]);
	  	Delay_ms(t1);
		printf("%d\r\n",trans[i+3]);
		
		i+=4;
		j++;
		
		if(j==(data_sum_Num/8)){i=0;j=0;}
			LED_GREEN;
			
				
			if(KEY1_flag==1) //�̰��˳�
			{
				KEY1_flag=0;  //���־
				Delay_ms(500);//�ȴ������󴥷�flag1��--�˳�����ģʽ
				LED_BLUE;
			}
	}

/*******/
#endif	
	}
}


/*tof050�������뺯��*/
//		VL6180X_Start_Range();
//		VL6180X_Poll_Range();
//		distance=VL6180_Read_Range();
//		VL6180X_Clear_Interrupt();
//		printf("���Ծ���Ϊ��%d\r\n",distance);


/*********************************************END OF FILE**********************/

