#include "compass.h"

extern Mailbox mb_compass;
extern msg_t mb_compass_buf[MB_COMPASS_MSG_SIZE];

msg_t ThreadCompass( void *arg )
{
	msg_t msg;
	float angle = 0;
	
	initCompass();
	
	while(TRUE)
	{
		angle = getAngle();
		msg = (msg_t)&angle;
		chMBPost(&mb_compass, msg, TIME_IMMEDIATE);
	}
}

void initCompass()
{
	uint8_t txbuf[2] = {0};
	
	txbuf[0] = 0x00;																												//Registre
	txbuf[1] = 0x78;																												//Donnée
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = 0x01;																												//Registre
	txbuf[1] = 0x20;																												//Donnée
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = 0x02;																												//Registre
	txbuf[1] = 0x80;																												//Donnée
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	chThdSleepMilliseconds(6);
}

float getAngle()
{
	uint8_t txbuf[2] = {0};
	uint8_t rxbuf[6] = {0};
	
	txbuf[0] = 0x03;																										//Donnée
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 1, rxbuf, 2, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	// To be confirm:
	return (float)(rxbuf[1]<<8|rxbuf[2])/65535*360;
}
