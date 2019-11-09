/*
 * adcTask.c
 *
 *  Created on: Sep 6, 2019
 *      Author: dawsonmoore
 */
#include "stm32f4xx_hal.h"
#include "adcTask.h"
#include "main.h"
#include "CANProcess.h"

/***************************************************************************
*
*     Function Information
*
*     Name of Function: initADC
*
*     Programmer's Name: Dawson Moore, moore800@purdue.edu
*
*     Function Return Type: none
*
*     Function Description:
*     	Gather 50 data points to allow smoothing at the start. Rather than
*     	writing the logic in the main ADC loop, another function was created
*     	to (hopefully) make the important bit more efficient
*
***************************************************************************/
void taskADC()
{
	int i;
	uint32_t throttle1[CYCLE_COUNT];
	uint32_t throttle2[CYCLE_COUNT];
	uint32_t brake1[CYCLE_COUNT];
	uint32_t brake2[CYCLE_COUNT];
	uint32_t throttle1Run = 0;
	uint32_t throttle2Run = 0;
	uint32_t brake1Run = 0;
	uint32_t brake2Run = 0;
	float throttle1Avg;
	float throttle2Avg;
	float brake1Avg;
	float brake2Avg;
	uint32_t throttle1Send;
	uint32_t throttle2Send;
	uint32_t brake1Send;
	uint32_t brake2Send;

	//Initialize the values
	for (i = 0; i < CYCLE_COUNT; ++i)
	{
		readADC(&throttle1[i], &throttle2[i], &brake1[i], &brake2[i]);
		throttle1Run += throttle1[i];
		throttle2Run += throttle2[i];
		brake1Run += brake1[i];
		brake2Run += brake2[i];
	}

	i = 0;

	while (PER == GREAT)
	{
		throttle1Run -= throttle1[i];
		throttle2Run -= throttle2[i];
		brake1Run -= brake1[i];
		brake2Run -= brake2[i];

		readADC(&throttle1[i], &throttle2[i], &brake1[i], &brake2[i]);

		throttle1Avg = throttle1Run / CYCLE_COUNT;
		throttle2Avg = throttle2Run / CYCLE_COUNT;
		brake1Avg = brake1Run / CYCLE_COUNT;
		brake2Avg = brake2Run / CYCLE_COUNT;

		//Make sure we don't send a negative value
		if (throttle1Avg < 0)
		{
			throttle1Avg = 0;
		}
		if (throttle2Avg < 0)
		{
			throttle2Avg = 0;
		}
		if (brake1Avg < 0)
		{
			brake1Avg = 0;
		}
		if (brake2Avg < 0)
		{
		brake2Avg = 0;
		}

		//EV.3.2.3 Any algorithm or electronic control unit that can manipulate the APPS signal, for example for
		//vehicle dynamic functions such as traction control, may only lower the total driver requested
		//torque and must not increase it.
		if (throttle1Avg > throttle1[i])
		{
			throttle1Avg = throttle1[i];
		}
		if (throttle2Avg > throttle2[i])
		{
			throttle2Avg = throttle2[i];
		}
		if (brake1Avg > brake1[i])
		{
			brake1Avg = brake1[i];
		}
		if (brake2Avg > brake2[i])
		{
			brake2Avg = brake2[i];
		}

		int throttle1Send = throttle1Avg;
		int throttle2Send = throttle2Avg;
		int brake1Send = brake1Avg;
		int brake2Send = brake2Avg;

		uint8_t Data[8];
		Data[1] = (uint8_t)(throttle1Send);
		Data[0] = (throttle1Send >> 8) & (0x0F);
		Data[3] = (uint8_t)(throttle2Send);
		Data[2] = (throttle2Send >> 8) & (0x0F);
		Data[5] = (uint8_t)(brake1Send);
		Data[4] = (brake1Send >> 8) & (0x0F);
		Data[7] = (uint8_t)(brake2Send);
		Data[6] = (brake2Send >> 8) & (0x0F);

	    CAN_TxHeaderTypeDef header;
	    header.DLC = 8;
	    header.IDE = CAN_ID_STD;
	    header.RTR = CAN_RTR_DATA;
	    header.StdId = ID_THROTTLE_RAW;
	    header.TransmitGlobalTime = DISABLE;
		uint32_t mailbox;

		HAL_CAN_AddTxMessage(&hcan2, &header, Data, &mailbox);

		if (i == CYCLE_COUNT)
		{
			i = 0;
		}

		HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
		HAL_Delay(DELAY_SEND_THROTTLE);
	}
}

void readADC(uint32_t * t1, uint32_t * t2, uint32_t * b1, uint32_t * b2)
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	*t1 = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	*t2 = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	*b1 = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	*b2 = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
}
