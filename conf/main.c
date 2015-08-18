#include "ch.h"
#include "hal.h"

static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg);

int main(void)
{
	/* OS init */
	halInit();
	chSysInit();
	
	/* Create one more task */
	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
	
	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{
		palSetPad(GPIOB, GPIOB_LED4);
		chThdSleepMilliseconds(100);
		palClearPad(GPIOB, GPIOB_LED4);
		chThdSleepMilliseconds(100);
	}
}

static msg_t Thread1(void *arg)
{
	/* Another task */
	while(TRUE)
	{
		palSetPad(GPIOB, GPIOB_LED3);
		chThdSleepMilliseconds(500);
		palClearPad(GPIOB, GPIOB_LED3);
		chThdSleepMilliseconds(500);
	}
}
