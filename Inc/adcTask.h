#ifndef ADCTASK_H
#define ADCTASK_H

#define CYCLE_COUNT						50 //How many values to smooth with
#define DELAY_SEND_THROTTLE				5 //Set delay to be 10 times less than previous pedalbox

typedef struct
{
	uint16_t throttle1[CYCLE_COUNT];
	uint16_t throttle2[CYCLE_COUNT];
	uint16_t brake1[CYCLE_COUNT];
	uint16_t brake2[CYCLE_COUNT];
	uint16_t throttle1Run; //Running total count [wastes memory, saves time]
	uint16_t throttle2Run;
	uint16_t brake1Run;
	uint16_t brake2Run;
	uint16_t throttle1Avg;
	uint16_t throttle2Avg;
	uint16_t brake1Avg;
	uint16_t brake2Avg;

	int cyclePos;
} Car_t;

extern volatile Car_t car;
extern ADC_HandleTypeDef hadc1;
extern CAN_HandleTypeDef hcan2;

void initADC();
void getADC();

#endif
