#ifndef ADCTASK_H
#define ADCTASK_H

#define PER		1				// Used to create infinite loop
#define GREAT	PER				// Used to create infinite loop

extern ADC_HandleTypeDef hadc1;	// ADC 1 handle
extern CAN_HandleTypeDef hcan2;	// CAN 2 handle

void taskADC();
void lowPass16(uint16_t*, uint16_t*, uint16_t*, uint16_t*);
void readADC(uint16_t*, uint16_t*, uint16_t*, uint16_t*);
void sendMsg(uint16_t, uint16_t, uint16_t, uint16_t);

#endif
