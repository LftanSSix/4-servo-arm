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
#include "./6180/6180.h"

 /**
  * @brief  TOF_Init，初始化TOF
  * @param  无
  * @retval 无
  */


uint8_t TOF_Init(void)
{
	uint8_t reset;
//	Delay_ms(200);	
	reset = Data_read_16mul(TOF_SLAVE_ADDRESS,0x016);
	
	if (reset==1){ // 检查是否初始化完成

		// 必须设置-专用寄存器 : private registers
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0207, 0x01);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0208, 0x01);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0096, 0x00);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0097, 0xfd);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00e3, 0x01);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00e4, 0x03);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00e5, 0x02);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00e6, 0x01);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00e7, 0x03);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00f5, 0x02);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00d9, 0x05);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00db, 0xce);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00dc, 0x03);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00dd, 0xf8);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x009f, 0x00);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00a3, 0x3c);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00b7, 0x00);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00bb, 0x3c);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00b2, 0x09);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00ca, 0x09);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0198, 0x01);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x01b0, 0x17);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x01ad, 0x00);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x00ff, 0x05);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0100, 0x05);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0199, 0x05);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x01a6, 0x1b);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x01ac, 0x3e);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x01a7, 0x1f);
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0030, 0x00);

	//建议设置-通用寄存器: Public registers 
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0011, 0x10); // Enables polling for ‘New Sample ready’ 
	 // when measurement completes
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x010a, 0x30); // Set the averaging sample period
	 // (compromise between lower noise and 
	 // increased execution time)
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x003f, 0x46); // Sets the light and dark gain (uppernibble). Dark gain should not be changed.

	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0031, 0xFF); // sets the # of range measurements after 
	 // which auto calibration of system is performed 
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0041, 0x63); // Set ALS integration time to 100ms

	Data_write_16mul(TOF_SLAVE_ADDRESS,0x002e, 0x01); // perform a single temperature calibration of the ranging sensor 
	
	//选择设置-通用寄存器: Public registers
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x001b, 0x09); // Set default ranging inter-measurement 
	 // period to 100ms
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x003e, 0x31); // Set default ALS inter-measurement period 
	 // to 500ms
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x0014, 0x24); // Configures interrupt on ‘New Sample 
	 // Ready threshold event’ 
		 
	Data_write_16mul(TOF_SLAVE_ADDRESS,0x016, 0x00); //标志位清零1to0 change fresh out of set status to 0
	
	printf("tof初始化成功\r\n");

	LED_RED;
	
	return 1;

}else
    return 0;
}

/*
    开始测量模式选择
    选择单次测量模式
*/
 uint8_t VL6180X_Start_Range()
  {
     Data_write_16mul(TOF_SLAVE_ADDRESS,0x018,0x01);      //01  03
	 return 0;
  }
  
uint16_t timeoutcnt=0; 
/*poll for new sample ready */
uint8_t VL6180X_Poll_Range()
{
    uint8_t status;
	  uint8_t range_status;
	  status=Data_read_16mul(TOF_SLAVE_ADDRESS,0x04f);
    range_status=status&0x07;
		while(range_status!=0x04)
		{
			  timeoutcnt++;
				if(timeoutcnt > 2)
				{
					break;
				}
				status=Data_read_16mul(TOF_SLAVE_ADDRESS,0x04f);
				range_status=status&0x07;
				Delay_ms(1);
			
		}		  
		return 0;
}
   
  
  /*read range result (mm)*/
 uint8_t VL6180_Read_Range()
  {
    int range;
	range=Data_read_16mul(TOF_SLAVE_ADDRESS,0x062);
	return range;
  }
  
  /*clear interrupt*/
  void VL6180X_Clear_Interrupt()
  {
     Data_write_16mul(TOF_SLAVE_ADDRESS,0x015,0x07); 
  }
  
  
  
  
  
  
  
  
  
  

 
