#include "ch.h"
#include "hal.h"
#include "alti.h"

// I2C Configuration
static const I2CConfig g_i2ccfg = 
{
	OPMODE_I2C,
	I2C_CLK_SPEED,
	FAST_DUTY_CYCLE_16_9
};

static WORKING_AREA(waAlti, 512);

int main(void)
{
	/* OS init */
	halInit();
	chSysInit();
	
	/*
	* I²C initialization
	*/
	//I²C 2
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SCL */
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SDA */
	i2cInit();
	i2cObjectInit(&I2CD1);
	i2cStart(&I2CD1, &g_i2ccfg);
	
	/* Create one more task */
	chThdCreateStatic(waAlti, sizeof(waAlti), NORMALPRIO, ThreadAlti, NULL);
	
	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{
		palSetPad(GPIOB, GPIOB_LED4);
		chThdSleepMilliseconds(100);
		palClearPad(GPIOB, GPIOB_LED4);
		chThdSleepMilliseconds(100);
	}
}


