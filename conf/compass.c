#include "compass.h"

msg_t ThreadCompass( void *arg )
{
	initCompass();
	
	while(TRUE)
	{
		getAngle();
	}
}

void initCompass()
{
	uint8_t txbuf[2] = {0};
	
	txbuf[0] = 0x00;																												//Registre
	txbuf[1] = 0x78;																												//Donn�e
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = 0x01;																												//Registre
	txbuf[1] = 0x20;																												//Donn�e
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = 0x02;																												//Registre
	txbuf[1] = 0x80;																												//Donn�e
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	//chThdSleepMilliseconds(6);
}

float getAngle()
{
	uint8_t txbuf[2] = {0};
	uint8_t rxbuf[6] = {0};
	
	txbuf[0] = 0x03;																										//Donn�e
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 1, rxbuf, 2, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	// To be confirm:
	return (float)(rxbuf[1]<<8|rxbuf[2])/65535*360;
}
