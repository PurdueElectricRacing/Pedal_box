#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "UserTasks.h"
#include "CANProcess.h"

void taskBlink_LED()
{
	//vTaskDelay(5000); //TESTING1

	for(;;)
	{
		HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
		vTaskDelay(500);

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

//extern volatile uint16_t ADC1ConvertedValues[32];
extern ADC_HandleTypeDef hadc1;


void taskSendThrottleRaw() {
	//vTaskDelay(5000); //TESTING1
	for(;;) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		uint16_t value = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		uint16_t value2 = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		uint16_t value3 = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		uint16_t value4 = HAL_ADC_GetValue(&hadc1);

		HAL_ADC_Stop(&hadc1);
		CanTxMsgTypeDef tx;
		tx.StdId = 	ID_THROTTLE_RAW;
		tx.IDE =  	CAN_ID_STD;
		tx.RTR =	CAN_RTR_DATA;
		tx.DLC =  	8;
		tx.Data[1] = (uint8_t)(value);
		tx.Data[0] = (value >> 8) & (0x0F);
		tx.Data[3] = (uint8_t)(value2);
		tx.Data[2] = (value2 >> 8) & (0x0F);
		tx.Data[5] = (uint8_t)(value3);
		tx.Data[4] = (value3 >> 8) & (0x0F);
		tx.Data[7] = (uint8_t)(value4);
		tx.Data[6] = (value4 >> 8) & (0x0F);

		vTaskDelay(DELAY_SEND_THROTTLE);

		xQueueSendToBack(q_txcan, &tx, 100);
	}
	vTaskDelete(NULL);
}

/*void taskSendBrakeRaw() {
	for(;;) {
	    HAL_ADC_Start(&hadc1);
		CanTxMsgTypeDef tx;
		tx.StdId = 	ID_BRAKE_RAW;
		tx.IDE =  	CAN_ID_STD;
		tx.RTR =	CAN_RTR_DATA;
		tx.DLC =  	2;
		tx.Data[1] = (uint8_t)(ADC1ConvertedValues[2]);
		tx.Data[0] = (ADC1ConvertedValues[0] >> 8) & (0x0F);

		tx.Data[3] = (uint8_t)(ADC1ConvertedValues[3]);
		tx.Data[2] = (ADC1ConvertedValues[1] >> 8) & (0x0F);

		vTaskDelay(DELAY_SEND_BRAKE);

		xQueueSendToBack(q_txcan, &tx, 100);
	}
	vTaskDelete(NULL);
}*/
