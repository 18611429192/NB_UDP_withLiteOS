
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2020 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "los_sys.h"
#include "los_task.ph"
#include "los_memory.ph"
#include "los_base.h"
#include <string.h>
#include "los_sem.h"
#include <time.h>
#include "queue.h"
#include "hex_dec.h"
#include "los_swtmr.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define MAX_RCV_LEN 1024
uint8_t data[MAX_RCV_LEN];
uint8_t data2[MAX_RCV_LEN];
uint8_t reply_from_lpuart1[MAX_RCV_LEN];
uint8_t KEY1_flag=0;
uint8_t KEY2_flag=0;
uint8_t alive_flag=0;
uint16_t lpuart1_recv_len=0;
uint16_t uart1_recv_len=0;

//123.56.118.82
uint8_t cmd_CRE_SOC[]={"AT+NSOCR=DGRAM,17,0\r\n"};//通过NB创建SOCKET，端口号随机

uint8_t SERVER_IP[] = {"123.56.240.120"};
uint8_t SERVER_PORT[] = {"8887"};

uint8_t send_cmd[1024];

uint8_t cmd_msg_reply[]={"AT+NSONMI=3\r\n"};//接收到信息直接报告，格式"+NSONMI: <socket>,<length>,<data>"

uint8_t cmd_msg_read[20]={"AT+NSORF=2,2\r\n"};
uint8_t cmd_msg_notice[]={"NSONMI"};
uint8_t receive_msg[MAX_RCV_LEN]={"\0"};
uint8_t cmd_msg_clear[]={"AT+NSORF=2,1000\r\n"};


uint8_t OK[]={"OK"};
//uint8_t send_cmd[128]={"AT+NSOSD=0,"};

uint8_t send_cmd_udp[128]={"AT+NSOST=0,"};

uint8_t NB_socket[1];
uint8_t shuzi[]={"0123456789"};

uint8_t cmd_CGATT[]={"AT+CGATT?\r\n"};//查询是否注网
uint8_t CGATT[]={"+CGATT:1"};


//定义全局的信息标志
uint8_t message_flag = 0;
int message_n = 0;

//初始化消息队列
	sqQueue mes_Q;

//定义周期性的定时器
UINT16 id1;// timer id


void USART_IDLECallBack(void);
void LPUART_IDLECallBack(void);
int mysizeof(uint8_t* cmd);
uint8_t* mystrstr(uint8_t* src,uint8_t* sub);






UINT32 LED_Task_Handle;
UINT32 ALIVE_Task_Handle;
UINT32 udpsocket_Task_Handle;
UINT32 CGATT_Task_Handle;
UINT32 FIRST_READ_Task_Handle;
UINT32 THINK_Task_Handle;
UINT32 CLEAR_Task_Handle;
UINT32 COMPREHENSION_Task_Handle;

UINT32 SUB_Binary;
UINT32 UDP_Binary;
UINT32 READ_Binary;
UINT32 THINK_Binary;

uint8_t NB_create_tcpsocket()
{
	int i=0;
	for(i=0;i<=20;i++)
	{
		HAL_UART_Transmit(&hlpuart1,cmd_CRE_SOC,mysizeof(cmd_CRE_SOC),0xff);
		LOS_TaskDelay(1000);
		if(NULL!=mystrstr(reply_from_lpuart1,OK))
		{
			NB_socket[0]=reply_from_lpuart1[2];
			printf("创建socket成功\r\n");
			break;
		}
	}
	if(21==i)
	{
		printf("创建socket失败\r\n");
		return 0;
	}
	
	LOS_TaskDelay(1000);
	for(i=0;i<=20;i++)
	{
		HAL_UART_Transmit(&hlpuart1,cmd_msg_reply,mysizeof(cmd_msg_reply),0xff);
		LOS_TaskDelay(1000);
		if(NULL!=mystrstr(reply_from_lpuart1,OK))
		{
			printf("cmd_msg_reply成功\r\n");
			break;
		}
	}
	if(21==i)
	{
		printf("cmd_msg_reply失败\r\n");
		return 0;
	}
	
	
	return 1;
	
	
}

uint8_t send_UDP_to_Ali(uint8_t* NB_socket,int n)
{
	
	//send_cmd[]={"AT+NSOST=2,123.56.240.120,8887,2,3132\r\n"};
	
	uint8_t n_string[20];
	sprintf(n_string,"s%x",n);
	int n_size = (strlen(n_string)-1)/2;
	memset(send_cmd,0,1024);
	sprintf(send_cmd,"AT+NSOST=%s,%s,%s,%d,%s\r\n",NB_socket,SERVER_IP,SERVER_PORT,n_size+1,n_string);
	
	
	int i=0;
	int j=0;
	
	for(j=0;j<=20;j++)
	{
		HAL_UART_Transmit(&hlpuart1,send_cmd,mysizeof(send_cmd),0xff);
		LOS_TaskDelay(50);
		if(NULL!=mystrstr(reply_from_lpuart1,OK))
		{
			printf("发送msg成功\r\n");
			break;
		}
	}
	if(21==j)
	{
		printf("发送msg失败\r\n");
		return 0;
	}

		
	return 1;
}




void LED_task()
{
	while(1)
	{
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		LOS_TaskDelay(10000);
	}
}


void alive_task()
{
	while(1)
	{
		LOS_TaskDelay(60000+(rand()%1000));
		if(send_UDP_to_Ali(NB_socket,message_n) == 1){
			printf("sendACK%d\n",message_n);
			
		}
	}
}

void comprehension_task()
{
	while(1)
	{
		LOS_TaskDelay(10);
		char temp[100];
		while(DeQueue(&mes_Q,temp)){
			if(mystrstr(temp,"+NSONMI")){
				int n = Fixed_key(temp+13);
				if(n<message_n){
					continue;
				}
				printf("ACK%d\n",n);
				message_n +=1;
			}
			memset(temp,0,100);
		}
	}
}

void timer1_callback()
{
	if(send_UDP_to_Ali(NB_socket,message_n) == 1){
			printf("补发\r\n");
	}
}






void udpsocket_task()
{
	while(1)
	{
		LOS_SemPend(UDP_Binary,LOS_WAIT_FOREVER);
		uint8_t uwet=NB_create_tcpsocket();
		if(uwet==1)
			LOS_TaskResume(ALIVE_Task_Handle);
	}
}

void CGATT_task()
{
	while(1)
	{
		LOS_TaskDelay(5000);
		for(int i=0;i<10;i++)
		{
			HAL_UART_Transmit(&hlpuart1,cmd_CGATT,mysizeof(cmd_CGATT),0xff);
			LOS_TaskDelay(1000);
			if(NULL!=mystrstr(reply_from_lpuart1,CGATT))
			{
				LOS_SemPost(UDP_Binary);
				LOS_TaskSuspend(CGATT_Task_Handle);
			}
		}
	}
}


static UINT32 AppTaskCreate(void)
{
	
	UINT32 uwRet = LOS_OK;
	
	
	
	uwRet=LOS_SemCreate(1,&UDP_Binary);
	if (uwRet != LOS_OK)
	{
			printf("UDP_Binary create failed,%X\n",uwRet);
			return uwRet;
	}
	
	
	TSK_INIT_PARAM_S task_init_param;
	
	task_init_param.usTaskPrio = 6;
	task_init_param.pcName = "LEDTask";
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LED_task;
	task_init_param.uwStackSize = 1024;
	uwRet = LOS_TaskCreate(&LED_Task_Handle, &task_init_param);
	if (uwRet != LOS_OK)
	{
			printf("LEDtask create failed,%X\n",uwRet);
			return uwRet;
	}
	
	
	task_init_param.usTaskPrio = 5;
	task_init_param.pcName = "aliveTask";
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)alive_task;
	task_init_param.uwStackSize = 1024;
	uwRet = LOS_TaskCreate(&ALIVE_Task_Handle, &task_init_param);
	if (uwRet != LOS_OK)
	{
			printf("ALIVEtask create failed,%X\n",uwRet);
			return uwRet;
	}
	

	
	task_init_param.usTaskPrio = 3;
	task_init_param.pcName = "UDPTask";
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)udpsocket_task;
	task_init_param.uwStackSize = 1024;
	uwRet = LOS_TaskCreate(&udpsocket_Task_Handle, &task_init_param);
	if (uwRet != LOS_OK)
	{
			printf("udptask create failed,%X\n",uwRet);
			return uwRet;
	}
	
	task_init_param.usTaskPrio = 3;
	task_init_param.pcName = "CGATTTask";
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)CGATT_task;
	task_init_param.uwStackSize = 1024;
	uwRet = LOS_TaskCreate(&CGATT_Task_Handle, &task_init_param);
	if (uwRet != LOS_OK)
	{
			printf("CGATTTask create failed,%X\n",uwRet);
			return uwRet;
	}
	
	task_init_param.usTaskPrio = 4;
	task_init_param.pcName = "comprehension_task";
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)comprehension_task;
	task_init_param.uwStackSize = 1024;
	uwRet = LOS_TaskCreate(&COMPREHENSION_Task_Handle, &task_init_param);
	if (uwRet != LOS_OK)
	{
			printf("COMPREHENSION_Task create failed,%X\n",uwRet);
			return uwRet;
	}
	
	
	return LOS_OK;
}




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_LPUART1_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

	memset(data,0,MAX_RCV_LEN);
	memset(data2,0,MAX_RCV_LEN);
	memset(reply_from_lpuart1,0,MAX_RCV_LEN);
	
	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&hlpuart1,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1,data2,MAX_RCV_LEN);
	HAL_UART_Receive_DMA(&hlpuart1,data,MAX_RCV_LEN);

//创建一个随机种子，解决函数中因为定时所可能出现的碰撞问题，
  srand((unsigned)time(NULL));
	
	
	
//初始化消息队列
	InitQueue(&mes_Q);



	UINT32 uwRet = LOS_OK;
	LOS_KernelInit();
  uwRet = AppTaskCreate();
  if(uwRet != LOS_OK) {
      printf("LOS Creat task failed\r\n");
      //return LOS_NOK;
  }
	else printf("LOS Creat task OK\r\n");
	
	
	LOS_SemPend(UDP_Binary,50);

	LOS_TaskSuspend(ALIVE_Task_Handle);

	
	LOS_Start();









  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure LSE Drive Capability 
    */
  HAL_PWR_EnableBkUpAccess();

  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /**Enable MSI Auto calibration 
    */
  HAL_RCCEx_EnableMSIPLLMode();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* LPUART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(LPUART1_IRQn);
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/* USER CODE BEGIN 4 */




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		
	}
	if(huart->Instance==LPUART1)
	{
		
	}
}


void USART_IDLECallBack()
{

	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	HAL_UART_DMAStop(&huart1);
	int len=0;
	len=mysizeof(data2);
	HAL_UART_Transmit(&hlpuart1,data2,len,0xff);
	memset(data2,0,MAX_RCV_LEN);
	HAL_UART_Receive_DMA(&huart1,data2,MAX_RCV_LEN);
	
}

void LPUART_IDLECallBack()
{
	__HAL_UART_CLEAR_IDLEFLAG(&hlpuart1);
	LOS_TaskDelay(20);
	HAL_UART_DMAStop(&hlpuart1);
	printf("data:%s",data);
	
	EnQueue(&mes_Q,data);
	memcpy(reply_from_lpuart1,data,MAX_RCV_LEN);
	memset(data,0,MAX_RCV_LEN);
	HAL_UART_Receive_DMA(&hlpuart1,data,MAX_RCV_LEN);
}





void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==KEY1_Pin)
	{
		
	}
	if(GPIO_Pin==KEY2_Pin)
	{
		
	}
}



int mysizeof(uint8_t* cmd)
{
	int len=0;
	for(int i=0;*(cmd+i)!='\r';i++)
	{
		len++;
	}
	len+=2;
	return len;
}


uint8_t* mystrstr(uint8_t* src,uint8_t* sub)
{
	
		uint8_t *bp;
    uint8_t *sp;
    if(!src||!sub)
    {
        return src;
    }
    /* 遍历src字符串  */
    while(*src)
    {
        /* 用来遍历子串 */
        bp=src;
        sp=sub;
        do
        {
          if(!*sp)  /*到了sub的结束位置，返回src位置   */
                return src;
        }while(*bp++==*sp++);
        src++;
    }
    return NULL;
	
}





int fputc (int ch,FILE* f)
{
	uint8_t temp[1]={ch};
	HAL_UART_Transmit(&huart1,temp,1,2);
	return HAL_OK;
}


time_t time(time_t *p)
  {
    return 0;
   
  }





/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
