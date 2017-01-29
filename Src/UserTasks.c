#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "UserTasks.h"

extern QueueHandle_t q_txcan;

void taskBlink_LED(int *ledID)
{
	for(;;)
	{
		GPIO_TypeDef* portToToggle;
		unsigned short pinToToggle;

		switch(*ledID) {
			case 4:
				//Green
				portToToggle = LD4_GPIO_Port;
				pinToToggle = LD4_Pin;
				break;
			case 3:
				//Orange
				portToToggle = LD3_GPIO_Port;
				pinToToggle = LD3_Pin;
				break;
			default:
				//Red
				portToToggle = LD5_GPIO_Port;
				pinToToggle = LD5_Pin;
				break;
		}

		HAL_GPIO_TogglePin(portToToggle, pinToToggle);
//		HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
		vTaskDelay(200/portTICK_RATE_MS);

		CanTxMsgTypeDef tx;
		tx.StdId = 	0x123;
		tx.IDE =  	CAN_ID_STD;
		tx.RTR =	CAN_RTR_DATA;
		tx.DLC =  	2;
		tx.Data[0] = 0xab;
		tx.Data[1] = 0xcd;

		xQueueSendToBack(q_txcan, &tx, 100);




	}
	vTaskDelete(NULL);
}
