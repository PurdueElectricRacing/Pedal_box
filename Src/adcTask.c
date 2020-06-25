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

// Local Variables
static const uint16_t tb_id = (TB_HLP << 26) | (TB_PGN << 6) | TB_SSA;	// Set CAN ID based on HLP, PGN, and SSA for future use
static const uint16_t  avFactor = ((1 << 15) / 5) << 1;					// Division factor for filter

/*
 * taskADC()
 *
 * Gathers raw ADC values, filters them (in compliance with rules), and sends
 * them off to PDU for processing
 *
 * Timing: Arbitrary (~1 ms)
 */
void taskADC()
{
	// Locals
	uint16_t 	t1[6];										// Raw ADC throttle 1 values in [16.0] counts
	uint16_t 	t2[6];										// Raw ADC throttle 2 values in [16.0] counts
	uint16_t 	b1[6];										// Raw ADC brake 1 values in [16.0] counts
	uint16_t 	b2[6];										// Raw ADC brake 2 values in [16.0] counts
	uint8_t		idx = 0;									// Current index in raw value arrays

	// Note:
	// EV.3.2.3 Any algorithm or electronic control unit that can manipulate the APPS signal, for example for
	// vehicle dynamic functions such as traction control, may only lower the total driver requested
	// torque and must not increase it.

	// We should request a rules clarification on this. If a low pass filter creates a slightly higher torque command,
	// that should be okay, correct? If not, that's stupid.

	while(PER == GREAT)										// Loop forever
	{
		readADC(&t1[idx], &t2[idx], &b1[idx], &b2[idx]);	                                // Gather new ADC values
		lowPass16(t1, t2, b1, b2);							        // Low pass filter ADC values
		sendMsg(t1[5], t2[5], b1[5], b2[5]);				                        // Send values (lowPass16 uses first 5 values to filter and stores in 6th)
		if(++idx == 5)										// Increment counter and reset when we've just accessed the last value
			idx = 0;									// Reset if we are at the upper bound

		HAL_Delay(20);										// Wait for at least 1 ms
	}
}

/*
 * lowPass16()
 *
 * Performs 5th order finite impulse response low pass filtering on gathered ADC count samples
 * Using boxcar FIR for now. Will update to fit proper transfer function when it's not 1:20 a.m.
 *
 * Timing: Arbitrary (~20 ms)
 */
void lowPass16(uint16_t* t1, uint16_t* t2, uint16_t* b1, uint16_t* b2)
{
	// Note: The [16.16] result of the multiplication will automatically have the low word pulled when implicitly casted back to uint16_t
	t1[5] = (t1[0] + t1[1] + t1[2] + t1[3] + t1[4]) * avFactor;	// [16.0] counts * [0.16] const = [16.16] counts
	t2[5] = (t2[0] + t2[1] + t2[2] + t2[3] + t2[4]) * avFactor;	// [16.0] counts * [0.16] const = [16.16] counts
	b1[5] = (b1[0] + b1[1] + b1[2] + b1[3] + b1[4]) * avFactor;	// [16.0] counts * [0.16] const = [16.16] counts
	b2[5] = (b2[0] + b2[1] + b2[2] + b2[3] + b2[4]) * avFactor;	// [16.0] counts * [0.16] const = [16.16] counts
}

/*
 * readADC()
 *
 * Gathers raw ADC values all in one go and passes them back to the calling function
 *
 * Timing: Arbitrary (~20 ms)
 */
void readADC(uint16_t* t1, uint16_t* t2, uint16_t* b1, uint16_t* b2)
{
	// ADC is 12 bit resolution, but regs are 32 bits wide. Therefore, we can just clip the upper word and send the lower word with no resolution loss
	HAL_ADC_Start(&hadc1);					// Start ADC 1
	HAL_ADC_PollForConversion(&hadc1, 10);	// Start polling samples
	*t1 = HAL_ADC_GetValue(&hadc1);			// Capture value in ADC register
	HAL_ADC_PollForConversion(&hadc1, 10);	// Start polling samples (will move to next ADC target automatically)
	*t2 = HAL_ADC_GetValue(&hadc1);			// Capture value in ADC register
	HAL_ADC_PollForConversion(&hadc1, 10);	// Start polling samples (will move to next ADC target automatically)
	*b1 = HAL_ADC_GetValue(&hadc1);			// Capture value in ADC register
	HAL_ADC_PollForConversion(&hadc1, 10);	// Start polling samples (will move to next ADC target automatically)
	*b2 = HAL_ADC_GetValue(&hadc1);			// Capture value in ADC register
	HAL_ADC_Stop(&hadc1);					// Stop ADC 1 (return to first ADC target)
}

/*
 * sendMsg()
 *
 * Sends CAN frame based on given throttle and brake values
 *
 * Timing: Arbitrary (~20 ms)
 */
void sendMsg(uint16_t t1, uint16_t t2, uint16_t b1, uint16_t b2)
{
	// Locals
	uint8_t 			data[8];			// Data buffer for message tx in [16.0] counts
	uint32_t 			mailbox;			// Mailbox that header frame will be placed into prior to tx
	CAN_TxHeaderTypeDef header;				// CAN header frame

	data[1] = t1;							// Gather low byte of ADC value
	data[0] = t1 >> 8;						// Gather second lowest byte of ADC value
	data[3] = t2;							// Gather low byte of ADC value
	data[2] = t2 >> 8;						// Gather second lowest byte of ADC value
	data[5] = b1;							// Gather low byte of ADC value
	data[4] = b1 >> 8;						// Gather second lowest byte of ADC value
	data[7] = b2;							// Gather low byte of ADC value
	data[6] = b2 >> 8;						// Gather second lowest byte of ADC value

	header.DLC = TB_LENGTH;					// Set length of data to tx
	header.IDE = CAN_ID_EXT;				// Set ID length to 29 bit (extended ID)
	header.RTR = CAN_RTR_DATA;				// Set frame type to data
	header.StdId = tb_id;					// Set CAN ID to HLP + PGN + SSA
	header.TransmitGlobalTime = DISABLE;

	HAL_CAN_AddTxMessage(&hcan2, &header, data, &mailbox);

	HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
}
