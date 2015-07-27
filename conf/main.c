#include "ch.h"
#include "hal.h"
#include "XBee.h"

static WORKING_AREA(waComSnd, 512);

int main(void)
{
	/* OS init */
	halInit();
	chSysInit();
	
	/* Create one more task */
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);

	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{
		chThdSleepMilliseconds(100);
	}
}

