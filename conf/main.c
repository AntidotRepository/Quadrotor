#include "ch.h"
#include "hal.h"
#include "compass.h"

// I2C Configuration
static const I2CConfig g_i2ccfg = 
{
	OPMODE_I2C,
	I2C_CLK_SPEED,
	FAST_DUTY_CYCLE_16_9
};

Mailbox mb_compass;
msg_t mb_compass_buf[MB_COMPASS_MSG_SIZE];

int main(void)
{
	msg_t msg;
	float *angle = NULL;
	
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
	
	chMBInit(&mb_compass, mb_compass_buf, MB_COMPASS_MSG_SIZE);
	
	/* Create one more task */
	chThdCreateStatic(waCompass, sizeof(waCompass), NORMALPRIO, ThreadCompass, NULL);
	
	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{
		chMBFetch(&mb_compass, &msg, TIME_IMMEDIATE);
		angle = (float*)msg;
		angle = angle;
	}
}
