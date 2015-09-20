#include "ch.h"
#include "hal.h"
#include "XBee.h"
#include "gyro.h"
#include "compass.h"

// I2C Configuration
static const I2CConfig g_i2ccfg = 
{
	OPMODE_I2C,
	I2C_CLK_SPEED,
	FAST_DUTY_CYCLE_16_9
};

Mailbox mb_XBee;
msg_t mb_XBee_buf[MB_XBEE_MSG_SIZE];
Mailbox mb_gyro;
msg_t mb_gyro_buf[MB_GYRO_MSG_SIZE];
Mailbox mb_compass;
msg_t mb_compass_buf[MB_COMPASS_MSG_SIZE];

int main(void)
{
	msg_t msg;
	DATA_COMM data_comm;
	
	volatile DATA_GYRO *data_gyro;
	float *angle = NULL;
	
	data_comm.altitude = 0;
	data_comm.battery = 0;
	data_comm.lacet = 0;
	data_comm.roulis = 0;
	data_comm.signal = 0;
	data_comm.tangage = 0;
	
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
	
	chMBInit(&mb_gyro, mb_gyro_buf, MB_GYRO_MSG_SIZE);
	chMBInit(&mb_compass, mb_compass_buf, MB_COMPASS_MSG_SIZE);
	
	/* Create tasks */
	chThdCreateStatic(waGyro, sizeof(waGyro), NORMALPRIO, ThreadGyro, NULL);
	chThdCreateStatic(waCompass, sizeof(waCompass), NORMALPRIO, ThreadCompass, NULL);
	
	// Delay creation of comm task to let time to sensors being initialized and not send shit over comm
	chThdSleepMilliseconds(1000);
	
	/* Create one more task */
	chMBInit(&mb_XBee, mb_XBee_buf, MB_XBEE_MSG_SIZE);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
	
	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{		
		chMBFetch(&mb_gyro, &msg, TIME_IMMEDIATE);
		data_gyro = (DATA_GYRO*)msg;
		
		chMBFetch(&mb_compass, &msg, TIME_IMMEDIATE);
		angle = (float*)msg;
		
		// Sleep mandatory. Without it, data are not well retrieve from the gyro mailbox
		chThdSleepMilliseconds(1);
		
		data_comm.tangage = data_gyro->angleTangage;
		data_comm.roulis = data_gyro->angleRoulis;
		data_comm.lacet = *angle;
		
		msg = (msg_t)&data_comm;
		chMBPost(&mb_XBee, msg, TIME_IMMEDIATE);
	}
}

