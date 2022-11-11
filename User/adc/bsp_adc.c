#include "./adc/bsp_adc.h"
#include "./systick/bsp_SysTick.h"
#include "./usart/bsp_debug_usart.h"
#include <math.h>


__IO uint16_t ADC_ConvertedValue[NumberOfChannel]={0};
__IO u16 aver_value[4]={0};

static void Rheostat_ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);// 使能 GPIO 时钟		
	
    GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN1|RHEOSTAT_ADC_GPIO_PIN2\
								  |RHEOSTAT_ADC_GPIO_PIN3|RHEOSTAT_ADC_GPIO_PIN4;// 配置通道4567 GPIO
								  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;   //不上拉不下拉
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

static void Rheostat_ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
    // 开启DMA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;	
    // 存储器地址，实际上就是一个内部SRAM的变量，	
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue; //数据从ADC_DR到内部SRAM
    // 数据传输方向为外设到存储器	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
	// 缓冲区大小为，指一次传输的数据量
  DMA_InitStructure.DMA_BufferSize = NumberOfChannel;	//4个通道，一次转换4个半字数据
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // 存储器地址固定,??存储地址递增
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    // // 外设数据大小为半字，即两个字节 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
    //	存储器数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    // DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
      // 禁止DMA FIFO	，使用直连模式
      DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
      // FIFO 大小，FIFO模式禁止时，这个不用配置	
      DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
      DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
      DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	// 选择 DMA 通道，通道存在于流中
	DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL;
	
    //初始化DMA流，流相当于一个大的管道，管道里面有很多通道
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
	// 使能DMA流
	DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);

/*是否时必须的？不配置时钟可以吗*/
  // -------------------ADC Common 结构体 参数 初始化------------------------
  // 开启ADC时钟
  RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
  // 独立ADC模式，一个ADC组
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // 时钟为fpclk 4分频，pclk2=HCLK/2=90MHZ
  // 得adc时钟频率=90/4=22.5MHZ
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
  // 禁止DMA直接访问模式	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // 采样时间间隔20个时钟周期	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);



    // -------------------ADC Init 结构体 参数 初始化--------------------------
   	ADC_StructInit(&ADC_InitStructure);
    // ADC 分辨率
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    // 扫描模式，多通道采集需要	
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
    // 连续转换	
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
      //禁止外部边沿触发
      ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
      //外部触发通道，本例子使用软件触发，此值随便赋值即可
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    //数据右对齐	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    //规则转换通道有四个
  ADC_InitStructure.ADC_NbrOfConversion = NumberOfChannel;
  
    ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
    //---------------------------------------------------------------------------
   	
  // 配置 ADC 通道转换顺序和采样时间周期，3个时钟周期（最快）。计算时（3+12个时钟周期）*1/22.5mhz=0.667us
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL1, 1, 
   	                       ADC_SampleTime_480Cycles);
    ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL2, 2, 
   	                         ADC_SampleTime_480Cycles); 
    ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL3, 3, 
   	                         ADC_SampleTime_480Cycles); 
    ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL4, 4, 
   	                         ADC_SampleTime_480Cycles); 

    // 使能DMA请求 after last transfer (Single-ADC mode)
    ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, ENABLE);
    // 使能ADC DMA
    ADC_DMACmd(RHEOSTAT_ADC, ENABLE);
   	// 使能ADC
    ADC_Cmd(RHEOSTAT_ADC, ENABLE);
	
    //开始adc转换，软件触发
    ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

/*
  *多次采样取平均值
  *转换读取值
*/
u16 get_aver_value(void)
{
	u8 i,j;
	u16 temp[4]={0};
	for(j=0;j<4;j++)
	{
		for(i=0;i<10;i++)
		{
			ADC_DMACmd(RHEOSTAT_ADC, DISABLE); //读取数据前先关闭DMA传输
			temp[j]+=ADC_ConvertedValue[j];
		    ADC_DMACmd(RHEOSTAT_ADC, ENABLE);  //读取完一次数据后，开启DMA传输（新的读取流程）
//			printf("%d\r\n",ADC_ConvertedValue[0]);
//			Delay_ms(1);
	    }	
			aver_value[j]=(u16)((306*temp[j]/10)/4096+103);  //PCA9685 12位计数器（4096）对应0.5-2.5ms脉宽
				
				
	}
	 return aver_value[j];//,aver_value[1],aver_value[2],aver_value[3];

}

void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}



