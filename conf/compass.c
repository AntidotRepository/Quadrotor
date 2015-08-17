#include "compass.h"

extern Mailbox mb_compass;
extern msg_t mb_compass_buf[MB_COMPASS_MSG_SIZE];

msg_t ThreadCompass( void *arg )
{
	msg_t msg;
	int angle = 0;
	
	initCompass();
	
	while(TRUE)
	{
		angle = getAngle();
		msg = (msg_t)&angle;
		chMBPost(&mb_compass, msg, TIME_IMMEDIATE);
		
		chThdSleepMilliseconds(70);
	}
}

void initCompass()
{
	uint8_t txbuf[2] = {0};
	
	txbuf[1] = 0;	// Clean TxBuf
	txbuf[0] = HMC_CONF_REGISTER_A;
	txbuf[1] = HMC_NUM_8_SAMPLES | HMC_OUTPUT_RATE_15Hz | HMC_NORMAL_MEASUR_MODE;
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, HMC_I2C_ADDRESS, txbuf, 2, NULL, 0, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	txbuf[1] = 0;	// Clean TxBuf
	txbuf[0] = HMC_CONF_REGISTER_B;
	txbuf[1] = HMC_GAIN_CONF_1090;
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, HMC_I2C_ADDRESS, txbuf, 2, NULL, 0, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	txbuf[1] = 0;	// Clean TxBuf
	txbuf[0] = HMC_MODE_REGISTER;
	txbuf[1] = HMC_I2C_NORMAL_SPEED | HMC_CONTINUOUS_MEAS_MODE;
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, HMC_I2C_ADDRESS, txbuf, 2, NULL, 0, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	chThdSleepMilliseconds(6);
}

int16_t getAngle()
{
	long X, Y, AX, AY, ival, oval, aival;
	int8_t quad = 0;
	uint8_t txbuf[2] = {0};
	uint8_t rxbuf[6] = {0};
	
	txbuf[0] = 0x09;		// Status register
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, HMC_I2C_ADDRESS, txbuf, 1, rxbuf, 1, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	if((rxbuf[0]&0x02) == 1)
	{
		rxbuf[0] = 0;
		rxbuf[1] = 0;
	}
	else if((rxbuf[0]&0x01) == 1)
	{
		txbuf[0] = 0x03;
		i2cAcquireBus(&I2CD1);																									
		i2cMasterTransmitTimeout(&I2CD1, HMC_I2C_ADDRESS, txbuf, 1, rxbuf, 6, I2C_TIMEOUT);	// Mandatory to read all 6 bytes to not lock compass!!!
		i2cReleaseBus(&I2CD1);
	}
	X = (long)((rxbuf[0]<<8)|rxbuf[1]);
	if(X>0x8000)
		X-=0xFFFF;
	X*=0.92;
	Y = (float)((rxbuf[4]<<8)|rxbuf[5]);
	if(Y>0x8000)
		Y-=0xFFFF;
	Y*=0.92;

	AX = fabs(X); // angle X
	AY = fabs(Y); // angle Y
	
	//http://www.ccsinfo.com/forum/viewtopic.php?t=50920
	//Now the approximation used works for tan from -1 to 1, so we have to keep the 
  //values inside this range and adjust the input/output. 
  //Four 'quadrants' are decoded -1 to 1, (315 to 45 degrees), then 45 to 135,
  //135 to 225, 225 to 315 
	
	if(X >= 0) // Right hand half of the circule
	{
		if(AY > X)
		{
			if(Y < 0)
			{
				quad = 4;
				ival = -X/Y;
			}
			else
			{
				quad = 2;
				ival = X/-Y;
			}
		}
		else
		{
			if(AY > X)
			{
				quad = 4;
				ival = -Y/X;
			}
			else
			{
				quad = 1;
				ival = Y/X;
			}
		}
	}
	else
	{
		// Now the others
		if(Y > AX)
		{
			quad = 2;
			ival = X/-Y;
		}
		else
		{
			if(AY > AX)
			{
				quad = 4;
				ival = -X/Y;
			}
			else
			{
				quad = 3;
				ival = -Y/-X;
			}
		}
	}
	aival = fabs(ival);
	oval = 45*ival-ival*(aival-1)*(14.02+3.7*aival);
	if(quad != 1)
	{
		if(quad == 2)
		{
			oval = oval+90;
		}
		else
		{
			if(quad == 3)
			{
				oval = oval + 180;
			}
			else
			{
				oval = oval + 270;
			}
		}
	}
	if(oval < 0)
	{
		oval += 360;
	}
/*	if(angle >  32768)
		angle = (65536-angle)*(-1);*/
	// To be confirm:
	return oval;
}
