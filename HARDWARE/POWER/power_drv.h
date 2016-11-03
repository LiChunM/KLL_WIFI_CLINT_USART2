#ifndef _POWER_DRV_H_
#define _POWER_DRV_H_
#include "sys.h"

#define DRV_ALARM_ON		GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define DRV_ALARM_OFF	GPIO_ResetBits(GPIOA,GPIO_Pin_8)

#define DRV_LEDBE_ON		GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define DRV_LEDLBE_OFF	GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define DRV_LEDFR_OFF		GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define DRV_LEDFR_ON		GPIO_SetBits(GPIOB,GPIO_Pin_12)


#define DRV_WIFI_RST_ONE		GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define DRV_WIFI_RST_ZERO	GPIO_ResetBits(GPIOA,GPIO_Pin_1)

#define DRV_5V_ON	GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define DRV_5V_OFF	GPIO_ResetBits(GPIOB,GPIO_Pin_1)


#define  SW_CH3 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_14)
#define  SW_CH1 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)

#define  SW_IN 	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)
#define  SW_CH2 	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)




extern u8 Config_Byte;
void power_drv_init(void);
void ACC_Init(void);
void Sound_drv_init(void);
u8 send_Alarm(u8 senddata);

#endif

