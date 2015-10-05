#include "ch.h"
#include "hal.h"
#include "XBee.h"
#include "gyro.h"
#include "compass.h"
#include "alti.h"
#include "stabi.h"
#include "motors.h"

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
Mailbox mb_alti;
msg_t mb_alti_buf[MB_ALTI_MSG_SIZE];
Mailbox mb_stabi_state;
msg_t mb_stabi_buf[MB_STABI_MSG_SIZE];
Mailbox mb_stabi_state_d;
msg_t mb_stabi_d_buf[MB_STABI_MSG_SIZE];
Mailbox mb_stabi_motor;
msg_t mb_stabi_motor_buf[MB_STABI_MOTOR_MSG_SIZE];

int main(void)
{
	msg_t msg_gyro;
	msg_t msg_compass;
	msg_t msg_alti;
	msg_t msg_XBee;
	msg_t msg_stabi;
	msg_t msg_stabi_d;
	
	DATA_COMM data_comm;
	DATA_GYRO *data_gyro;
	float *north = NULL;
	float *altitude;
	DATA_STABI data_stabi;
	DATA_STABI data_stabi_d;
	
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
	
	palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(2));	// For debug only
	
	chMBInit(&mb_gyro, mb_gyro_buf, MB_GYRO_MSG_SIZE);
	chMBInit(&mb_compass, mb_compass_buf, MB_COMPASS_MSG_SIZE);
	chMBInit(&mb_alti, mb_alti_buf, MB_ALTI_MSG_SIZE);
	/* Create tasks */
	chThdCreateStatic(waGyro, sizeof(waGyro), NORMALPRIO, ThreadGyro, NULL);
	chThdCreateStatic(waCompass, sizeof(waCompass), NORMALPRIO, ThreadCompass, NULL);
	chThdCreateStatic(waAlti, sizeof(waAlti), NORMALPRIO, ThreadAlti, NULL);
	
	// Delay creation of comm task to let time to sensors being initialized and not send shit over comm
	chThdSleepMilliseconds(1000);
	
	/* Create more tasks */
	chMBInit(&mb_XBee, mb_XBee_buf, MB_XBEE_MSG_SIZE);
	chMBInit(&mb_stabi_state, mb_stabi_buf, MB_STABI_MSG_SIZE);
	chMBInit(&mb_stabi_state_d, mb_stabi_d_buf, MB_STABI_MSG_SIZE);
	
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
	chThdCreateStatic(waStabi, sizeof(waStabi), NORMALPRIO, ThreadStabi, NULL);
	
	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{
		chMBFetch(&mb_gyro, &msg_gyro, TIME_IMMEDIATE);
		data_gyro = (DATA_GYRO*)msg_gyro;
		
		chMBFetch(&mb_compass, &msg_compass, TIME_IMMEDIATE);
		north = (float*)msg_compass;
		
		if( *north != *north)
			__NOP();
				
		chMBFetch(&mb_alti, &msg_alti, TIME_IMMEDIATE);
		altitude=(float*)msg_alti;
		
		// Sleep mandatory. Without it, data are not well retrieve from the gyro mailbox
		chThdSleepMilliseconds(10);
		
		data_comm.tangage = data_gyro->angleTangage;
		data_comm.roulis = data_gyro->angleRoulis;
		data_comm.lacet = *north;
		data_comm.altitude = *altitude;
		
		data_stabi.dotPosX = 0;
		data_stabi.dotPosY = 0;
		data_stabi.dotPosZ = 0;
		data_stabi.dotPitch = data_gyro->accTangage;
		data_stabi.dotRoll = data_gyro->accRoulis;
		data_stabi.dotYaw = data_gyro->accLacet;
		data_stabi.posX = 0;
		data_stabi.posY = 0;
		data_stabi.posZ = *altitude;
		data_stabi.pitch = data_gyro->angleTangage;
		data_stabi.roll = data_gyro->angleRoulis;
		data_stabi.yaw = data_gyro->angleLacet;
		
		msg_stabi = (msg_t)&data_stabi;
		chMBPost(&mb_stabi_state, msg_stabi, TIME_IMMEDIATE);
		
#ifdef DEBUG_WARNINGS		
#warning This values should come from XBee task in the future
#endif
		data_stabi_d.dotPosX = 0;
		data_stabi_d.dotPosY = 0;
		data_stabi_d.dotPosZ = 0;
		data_stabi_d.dotPitch = 0;
		data_stabi_d.dotRoll = 0;
		data_stabi_d.dotYaw = 0;
		data_stabi_d.posX = 0;
		data_stabi_d.posY = 0;
		data_stabi_d.posZ = -1;
		data_stabi_d.pitch = 0;
		data_stabi_d.roll = 0;
		data_stabi_d.yaw = 0;
		
		msg_stabi_d = (msg_t)&data_stabi_d;
		chMBPost(&mb_stabi_state_d, msg_stabi_d, TIME_IMMEDIATE);
		
		msg_XBee = (msg_t)&data_comm;
		chMBPost(&mb_XBee, msg_XBee, TIME_IMMEDIATE);
	}
}


