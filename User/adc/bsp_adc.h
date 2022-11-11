#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"
#include "./sys.h"


#define NumberOfChannel      4

/*=====================通道1 IO======================*/
// PC3 通过调帽接电位器
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT1    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN1     GPIO_Pin_4
#define RHEOSTAT_ADC_GPIO_CLK1     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL1      ADC_Channel_4

#define RHEOSTAT_ADC_GPIO_PORT2    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN2     GPIO_Pin_5
#define RHEOSTAT_ADC_GPIO_CLK2     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL2      ADC_Channel_5

#define RHEOSTAT_ADC_GPIO_PORT3    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN3     GPIO_Pin_6
#define RHEOSTAT_ADC_GPIO_CLK3     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL3     ADC_Channel_6

#define RHEOSTAT_ADC_GPIO_PORT4    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN4     GPIO_Pin_7
#define RHEOSTAT_ADC_GPIO_CLK4     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL4     ADC_Channel_7

#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1

// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC1+0x4c)


// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0


u16 get_aver_value(void);
void Rheostat_Init(void);

#endif /* __BSP_ADC_H */



