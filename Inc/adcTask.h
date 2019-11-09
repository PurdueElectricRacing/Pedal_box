#ifndef ADCTASK_H
#define ADCTASK_H

#define CYCLE_COUNT						50 //How many values to smooth with
#define DELAY_SEND_THROTTLE				5 //Set delay to be 10 times less than previous pedalbox

#define PER								1
#define GREAT							PER

extern ADC_HandleTypeDef hadc1;
extern CAN_HandleTypeDef hcan2;

void taskADC();
void readADC(uint32_t * t1, uint32_t * t2, uint32_t * b1, uint32_t * b2);

#endif
