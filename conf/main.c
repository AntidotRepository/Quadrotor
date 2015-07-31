#include "ch.h"
#include "hal.h"
#include "gyro.h"

// I2C Configuration
static const I2CConfig g_i2ccfg = 
{
	OPMODE_I2C,
	I2C_CLK_SPEED,
	FAST_DUTY_CYCLE_16_9
};

Mailbox mb_gyro;
msg_t mb_gyro_buf[MB_MSG_SIZE];

int main(void)
{
	msg_t msg;
	volatile DATA_GYRO *data;
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
	
	chMBInit(&mb_gyro, mb_gyro_buf, MB_MSG_SIZE);
	
	/* Create tasks */
	chThdCreateStatic(waGyro, sizeof(waGyro), NORMALPRIO, ThreadGyro, NULL);
	
	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{
		chMBFetch(&mb_gyro, &msg, TIME_INFINITE);
		data = (DATA_GYRO*)msg;
		chThdSleepMilliseconds(100);
	}
}

