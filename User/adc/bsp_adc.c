#include "./adc/bsp_adc.h"
#include "./systick/bsp_SysTick.h"
#include "./usart/bsp_debug_usart.h"
#include <math.h>


__IO uint16_t ADC_ConvertedValue[NumberOfChannel]={0};
__IO u16 aver_value[4]={0};

static void Rheostat_ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);// ʹ�� GPIO ʱ��		
	
    GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN1|RHEOSTAT_ADC_GPIO_PIN2\
								  |RHEOSTAT_ADC_GPIO_PIN3|RHEOSTAT_ADC_GPIO_PIN4;// ����ͨ��4567 GPIO
								  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;   //������������
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

static void Rheostat_ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
    // ����DMAʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
	// �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;	
    // �洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı�����	
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue; //���ݴ�ADC_DR���ڲ�SRAM
    // ���ݴ��䷽��Ϊ���赽�洢��	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
	// ��������СΪ��ָһ�δ����������
  DMA_InitStructure.DMA_BufferSize = NumberOfChannel;	//4��ͨ����һ��ת��4����������
	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // �洢����ַ�̶�,??�洢��ַ����
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    // // �������ݴ�СΪ���֣��������ֽ� 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
    //	�洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	// ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    // DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
      // ��ֹDMA FIFO	��ʹ��ֱ��ģʽ
      DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
      // FIFO ��С��FIFOģʽ��ֹʱ�������������	
      DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
      DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
      DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	// ѡ�� DMA ͨ����ͨ������������
	DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL;
	
    //��ʼ��DMA�������൱��һ����Ĺܵ����ܵ������кܶ�ͨ��
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
	// ʹ��DMA��
	DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);

/*�Ƿ�ʱ����ģ�������ʱ�ӿ�����*/
  // -------------------ADC Common �ṹ�� ���� ��ʼ��------------------------
  // ����ADCʱ��
  RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
  // ����ADCģʽ��һ��ADC��
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // ʱ��Ϊfpclk 4��Ƶ��pclk2=HCLK/2=90MHZ
  // ��adcʱ��Ƶ��=90/4=22.5MHZ
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
  // ��ֹDMAֱ�ӷ���ģʽ	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // ����ʱ����20��ʱ������	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);



    // -------------------ADC Init �ṹ�� ���� ��ʼ��--------------------------
   	ADC_StructInit(&ADC_InitStructure);
    // ADC �ֱ���
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    // ɨ��ģʽ����ͨ���ɼ���Ҫ	
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
    // ����ת��	
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
      //��ֹ�ⲿ���ش���
      ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
      //�ⲿ����ͨ����������ʹ�������������ֵ��㸳ֵ����
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    //�����Ҷ���	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    //����ת��ͨ�����ĸ�
  ADC_InitStructure.ADC_NbrOfConversion = NumberOfChannel;
  
    ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
    //---------------------------------------------------------------------------
   	
  // ���� ADC ͨ��ת��˳��Ͳ���ʱ�����ڣ�3��ʱ�����ڣ���죩������ʱ��3+12��ʱ�����ڣ�*1/22.5mhz=0.667us
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL1, 1, 
   	                       ADC_SampleTime_480Cycles);
    ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL2, 2, 
   	                         ADC_SampleTime_480Cycles); 
    ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL3, 3, 
   	                         ADC_SampleTime_480Cycles); 
    ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL4, 4, 
   	                         ADC_SampleTime_480Cycles); 

    // ʹ��DMA���� after last transfer (Single-ADC mode)
    ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, ENABLE);
    // ʹ��ADC DMA
    ADC_DMACmd(RHEOSTAT_ADC, ENABLE);
   	// ʹ��ADC
    ADC_Cmd(RHEOSTAT_ADC, ENABLE);
	
    //��ʼadcת�����������
    ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

/*
  *��β���ȡƽ��ֵ
  *ת����ȡֵ
*/
u16 get_aver_value(void)
{
	u8 i,j;
	u16 temp[4]={0};
	for(j=0;j<4;j++)
	{
		for(i=0;i<10;i++)
		{
			ADC_DMACmd(RHEOSTAT_ADC, DISABLE); //��ȡ����ǰ�ȹر�DMA����
			temp[j]+=ADC_ConvertedValue[j];
		    ADC_DMACmd(RHEOSTAT_ADC, ENABLE);  //��ȡ��һ�����ݺ󣬿���DMA���䣨�µĶ�ȡ���̣�
//			printf("%d\r\n",ADC_ConvertedValue[0]);
//			Delay_ms(1);
	    }	
			aver_value[j]=(u16)((306*temp[j]/10)/4096+103);  //PCA9685 12λ��������4096����Ӧ0.5-2.5ms����
				
				
	}
	 return aver_value[j];//,aver_value[1],aver_value[2],aver_value[3];

}

void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}



