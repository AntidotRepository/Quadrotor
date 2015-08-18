#include "ch.h"
#include "hal.h"
#include "XBee.h"
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
Mailbox mb_XBee;
msg_t mb_XBee_buf[MB_XBEE_MSG_SIZE];

int main(void)
{
	msg_t msg;
	DATA_COMM data_comm;
	int *angle = NULL;
	
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
	
	// Delay creation of comm task to let time to sensors being initialized and not send shit over comm
	chThdSleepMilliseconds(1000);
	chMBInit(&mb_XBee, mb_XBee_buf, MB_XBEE_MSG_SIZE);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
	
	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{
		chMBFetch(&mb_compass, &msg, TIME_IMMEDIATE);
#warning Add a condition to check if we get something in the mailbox... Or maybe useless... #tbc
		angle = (int*)msg;
		angle = angle;
		data_comm.lacet = *angle;
		msg = (msg_t)&data_comm;
		chMBPost(&mb_XBee, msg, TIME_IMMEDIATE);
		
		chThdSleepMilliseconds(100);
	}
}

		
