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

void initADC()
{
	int currentCycles;
	car.cyclePos;
	for (currentCycles = 0; currentCycles < CYCLE_COUNT; ++currentCycles)
	{
		HAL_ADC_Start(&hadc1);

		HAL_ADC_PollForConversion(&hadc1, 10);
		car.throttle1[currentCycles] = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		car.throttle2[currentCycles] = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		car.brake1[currentCycles] = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		car.brake2[currentCycles] = HAL_ADC_GetValue(&hadc1);

		HAL_ADC_Stop(&hadc1);

		car.throttle1Run += car.throttle1[currentCycles];
		car.throttle2Run += car.throttle2[currentCycles];
		car.brake1Run += car.brake1[currentCycles];
		car.brake2Run += car.brake2[currentCycles];
	}
	car.throttle1Avg = car.throttle1Run / CYCLE_COUNT;
	car.throttle2Avg = car.throttle2Run / CYCLE_COUNT;
	car.brake1Avg = car.brake1Run / CYCLE_COUNT;
	car.brake2Avg = car.brake2Run / CYCLE_COUNT;
}

void getADC()
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	car.throttle1[car.cyclePos] = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	car.throttle2[car.cyclePos] = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	car.brake1[car.cyclePos] = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	car.brake2[car.cyclePos] = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	if (car.cyclePos == 0)
	{
		car.throttle1Run += car.throttle1[car.cyclePos] - car.throttle1[CYCLE_COUNT - 1];
		car.throttle2Run += car.throttle2[car.cyclePos] - car.throttle2[CYCLE_COUNT - 1];
		car.brake1Run += car.brake1[car.cyclePos] - car.brake1[CYCLE_COUNT - 1];
		car.brake2Run += car.brake2[car.cyclePos] - car.brake2[CYCLE_COUNT - 1];
	}
	else
	{
		car.throttle1Run += car.throttle1[car.cyclePos] - car.throttle1[car.cyclePos - 1];
		car.throttle2Run += car.throttle2[car.cyclePos] - car.throttle2[car.cyclePos - 1];
		car.brake1Run += car.brake1[car.cyclePos] - car.brake1[car.cyclePos - 1];
		car.brake2Run += car.brake2[car.cyclePos] - car.brake2[car.cyclePos - 1];
	}
	car.throttle1Avg = car.throttle1Run / CYCLE_COUNT;
	car.throttle2Avg = car.throttle2Run / CYCLE_COUNT;
	car.brake1Avg = car.brake1Run / CYCLE_COUNT;
	car.brake2Avg = car.brake2Run / CYCLE_COUNT;

	uint8_t Data[8];
	Data[1] = (uint8_t)(car.throttle1Avg);
	Data[0] = (car.throttle1Avg >> 8) & (0x0F);
	Data[3] = (uint8_t)(car.throttle2Avg);
	Data[2] = (car.throttle2Avg >> 8) & (0x0F);
	Data[5] = (uint8_t)(car.brake1Avg);
	Data[4] = (car.brake1Avg >> 8) & (0x0F);
	Data[7] = (uint8_t)(car.brake2Avg);
	Data[6] = (car.brake2Avg >> 8) & (0x0F);
    CAN_TxHeaderTypeDef header;
    header.DLC = 8;
    header.IDE = CAN_ID_STD;
    header.RTR = CAN_RTR_DATA;
    header.StdId = ID_THROTTLE_RAW;
    header.TransmitGlobalTime = DISABLE;
	uint32_t mailbox;

	HAL_CAN_AddTxMessage(&hcan2, &header, Data, &mailbox);

	++car.cyclePos;
	if (car.cyclePos == CYCLE_COUNT)
	{
		car.cyclePos = 0;
	}
	HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
	HAL_Delay(DELAY_SEND_THROTTLE);
}
