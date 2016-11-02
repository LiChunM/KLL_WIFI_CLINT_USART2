#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "includes.h"  
#include "flash_in.h"
#include "analysis.h"
#include "stm32f10x_adc.h"
#include "analysis.h"
#include "adc.h"
#include "rc522.h"


//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//USART1任务
//设置任务优先级
#define USART1_TASK_PRIO       			3 
//设置任务堆栈大小
#define USART1_STK_SIZE  		    		512
//任务堆栈
OS_STK USART1_TASK_STK[USART1_STK_SIZE];
//任务函数处理用户的随时的输入
void usart1_task(void *pdata);

//USART2任务
//设置任务优先级
#define USART2_TASK_PRIO       		 	6
//设置任务堆栈大小
#define USART2_STK_SIZE  				512
//任务堆栈	
OS_STK USART2_TASK_STK[USART2_STK_SIZE];
//任务函数用来处理CDMA数据
void usart2_task(void *pdata);

//UART5任务
//设置任务优先级
#define UART5_TASK_PRIO    			2 
//设置任务堆栈大小
#define UART5_STK_SIZE  		 		256
//任务堆栈	
OS_STK UART5_TASK_STK[UART5_STK_SIZE];
//任务函数用来接受GPS数据
void uart5_task(void *pdata);


//主任务
//设置任务优先级
#define MAIN_TASK_PRIO       			8
//设置任务堆栈大小
#define MAIN_STK_SIZE  					512
//任务堆栈	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//任务函数
void main_task(void *pdata);


//预留任务
//设置任务优先级
#define XXX_TASK_PRIO       			5 
//设置任务堆栈大小
#define XXX_STK_SIZE  					64
//任务堆栈	
OS_STK XXX_TASK_STK[XXX_STK_SIZE];
//任务函数什么也不做
void xxx_task(void *pdata);

//HAND 任务
//设置任务优先级
#define HAND_TASK_PRIO      			7 //进行握手任务操作
//设置任务堆栈大小
#define HAND_STK_SIZE  				128
//任务堆栈	
OS_STK HAND_TASK_STK[HAND_STK_SIZE];
//任务函数
void hand_task(void *pdata);	
 			   
//323任务
//设置任务优先级
#define  MC323_TASK_PRIO       			4
//设置任务堆栈大小
#define MC323_STK_SIZE  					256
//任务堆栈	
OS_STK MC323_TASK_STK[MC323_STK_SIZE];
//任务函数什么也不做
void MC323_task(void *pdata);

 			   
//323任务
//设置任务优先级
#define  MC323_reconnection_TASK_PRIO       			9
//设置任务堆栈大小
#define MC323_reconnection_STK_SIZE  					128
//任务堆栈	
OS_STK MC323_reconnection_TASK_STK[MC323_reconnection_STK_SIZE];
//任务函数什么也不做
void MC323_reconnection_task(void *pdata);




 int main(void)
 {	 
 	JTAG_Set(0x02);
  	power_drv_init();
	ACC_Init();
	DRV_LEDR_OFF;
 	delay_init();	    	
	NVIC_Configuration();
	uart_init(72,115200);
	USART3_Init(115200);
	USART2_Init(36,115200);
	PWM_Init();
	TIM1_PWM_init(1500,16);
	InitRc522();
	IWDG_Init(4,625);
	SYS_Parameter_Init();
	OSInit();
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();
}							    
//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 	
	OSStatInit();					
 	OS_ENTER_CRITICAL();			    
 	OSTaskCreate(usart1_task,(void *)0,(OS_STK*)&USART1_TASK_STK[USART1_STK_SIZE-1],USART1_TASK_PRIO);						   
 	OSTaskCreate(usart2_task,(void *)0,(OS_STK*)&USART2_TASK_STK[USART2_STK_SIZE-1],USART2_TASK_PRIO);	 				   
 	OSTaskCreate(uart5_task,(void *)0,(OS_STK*)&UART5_TASK_STK[UART5_STK_SIZE-1],UART5_TASK_PRIO);
	OSTaskCreate(hand_task,(void *)0,(OS_STK*)&HAND_TASK_STK[HAND_STK_SIZE-1],HAND_TASK_PRIO);	 				   
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);	
	OSTaskCreate(MC323_task,(void *)0,(OS_STK*)&MC323_TASK_STK[MC323_STK_SIZE-1],MC323_TASK_PRIO);	 				   
 	OSTaskCreate(xxx_task,(void *)0,(OS_STK*)&XXX_TASK_STK[XXX_STK_SIZE-1],XXX_TASK_PRIO);
	OSTaskCreate(MC323_reconnection_task,(void *)0,(OS_STK*)&MC323_reconnection_TASK_STK[MC323_reconnection_STK_SIZE-1],MC323_reconnection_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);
	OS_EXIT_CRITICAL();
}
//usart1任务

void usart1_task(void *pdata)
{
	u8 res,t;
	pdata = pdata;
	while(1)
		{
			delay_ms(10);
			Usart1CommandAnalysis();
		}
}
//usart2任务
void usart2_task(void *pdata)
{	
	u8 status=0;
	pdata = pdata;
	while(1)
	{
		delay_ms(10);
		status = PcdRequest(PICC_REQALL,RC522CT);
		if(status==MI_OK)
			{
				printf("PcdRequest_MI_OK\r\n");
				printf("pTagType[0x%02X%02X]\r\n",RC522CT[0],RC522CT[1]);
				status=MI_ERR;
				status = PcdAnticoll(RC522SN);
				if(status==MI_OK)
					{
						status = MI_ERR;
						printf("RC522RDID:%02x %02x %02x %02x\r\n",RC522SN[0],RC522SN[1],RC522SN[2],RC522SN[3]);
					}
				delay_ms(1000);
				delay_ms(1000);
			}
		else
			{
				printf("PcdRequest_MI_ERROR,Next!!\r\n");
				delay_ms(1000);
			}
	}
}     
//gps任务
void uart5_task(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		delay_ms(4);
		moto_move();
	}									 
}
//主任务
void main_task(void *pdata)
{	
	u8 res;
	u8 lens=0;
	M35PowerOn();
 	while(1)
	{
		delay_ms(10);
		if(SystemFlow==0)
			{	
				Conecet2TheHandFromUdp();
				USART2_RX_STA=0;
				SystemFlow=1;
			}
	}
}	

void hand_task(void *pdata)
{
	u8 i;
	pdata=pdata;
	while(1)
		{
			delay_ms(100);	
		}
}
void MC323_task(void *pdata)
{
	
	pdata=pdata;
	while(1)
		{
			delay_ms(100);
			
			
		}
}
   		    
//监视任务
void xxx_task(void *pdata)
{	
	u8 i;
	pdata=pdata;
	while(1)
	{
		delay_ms(10);
		DRV_LEDL_ON;
		delay_ms(200);
		DRV_LEDL_OFF;
		for(i=0;i<20;i++)delay_ms(500);
		
	}
}

void MC323_reconnection_task(void *pdata)
{
	pdata=pdata;
	while(1)
		{
			delay_ms(20);
			IWDG_Feed();
		}
	
}


void HardFault_Handler(void)
{
	u32 temp;
	temp=SCB->CFSR;					
 	printf("CFSR:%8X\r\n",temp);	
	temp=SCB->HFSR;					
 	printf("HFSR:%8X\r\n",temp);	
 	temp=SCB->DFSR;					
 	printf("DFSR:%8X\r\n",temp);	
   	temp=SCB->AFSR;					
 	printf("AFSR:%8X\r\n",temp);
}





