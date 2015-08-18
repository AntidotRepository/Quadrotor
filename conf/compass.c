#include "compass.h"

extern Mailbox mb_compass;
extern msg_t mb_compass_buf[MB_COMPASS_MSG_SIZE];

enum states
{
	check,
	get,
	end
};


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
	#warning 1 - Add something to handle magnetic distortion due to the quadcopter body
	#warning 2 - For the moment, the north won't be given accurately when the compass (so the quadcopter) will tilt: Need to be done
	#warning 3 - Should be better to add something to compensate the magnetic declination (discussed here: http://bildr.org/2012/02/hmc5883l_arduino/)
	
	int state = check;
	uint8_t txbuf[2] = {0};
	uint8_t rxbuf[6] = {0};
	
	int16_t rawX = 0;
	int16_t rawY = 0;
	int16_t ArawX = fabs(rawX);
	int16_t ArawY = fabs(rawY);
	int8_t quad = 0;
	float ival, oval, aival;
	
	float head = 0;
	
	while(state != end)
	{
		switch(state)
		{
			case check:
				memset(txbuf, 0, 6*sizeof(uint8_t));
				txbuf[0] = HMC_STATUS_REGISTER;
				i2cAcquireBus(&I2CD1);																									
				i2cMasterTransmitTimeout(&I2CD1, HMC_I2C_ADDRESS, txbuf, 1, rxbuf, 1, I2C_TIMEOUT);
				i2cReleaseBus(&I2CD1);
			
				if((rxbuf[0] & HMC_DATA_OUTPUT_LOCK)  == 0 && (rxbuf[0] & HMC_DATA_OUTPUT_READY) == 1)
				{
					state = get;
				}
				break;
				
			case get:
				memset(txbuf, 0, 6*sizeof(uint8_t));
				txbuf[0] = HMC_DATA_OUTPUT_X_MSB;
				i2cAcquireBus(&I2CD1);
				i2cMasterTransmitTimeout(&I2CD1, HMC_I2C_ADDRESS, txbuf, 1, rxbuf, 6, I2C_TIMEOUT);
				i2cReleaseBus(&I2CD1);
			
				rawX = rxbuf[0]<<8 | rxbuf[1];
				rawY = rxbuf[4]<<8 | rxbuf[5];
				ArawX = fabs(rawX);												// We get absolute value of rawX
				ArawY = fabs(rawY);												// We get absolute value of rawY
			
				/*
					Routine to go faster than using atan
					Simply copy from this topic: http://www.ccsinfo.com/forum/viewtopic.php?t=50920
				*/
				if(rawX >= 0)
				{
					if(ArawY > rawX)
					{
						if(rawY < 0)
						{
							quad = 4;
							ival = (float)(-rawX) / (float)rawY;
						}
						else
						{
							quad = 2;
							ival = (float)rawX / (float)(-rawY);
						}
					}
					else
					{
						if(ArawY > rawX)
						{
							quad = 4;
							ival = (float)(-rawY) / (float)rawX;
						}
						else
						{
							quad = 1;
							ival = (float)rawY / (float)rawX;
						}
					}
				}
				else
				{
					if(rawY > ArawX)
					{
						quad = 2;
						ival = (float)rawX / (float)(-rawY);
					}
					else
					{
						if(ArawY > ArawX)
						{
							quad = 4;
							ival = (float)(-rawX) / (float)rawY;
						}
						else
						{
							quad = 3;
							ival = (float)(-rawY) / (float)(-rawX);
						}
					}
				}
				
				aival = fabs(ival);
				oval = 45 * ival - ival * (aival-1)*(14.02+3.79*aival);
				
				if(quad != 1)
				{
					if(quad == 2)
						oval = oval+90;
					else
					{
						if(quad == 3)
						{
							oval = oval+180;
						}
						else
						{
							oval = oval+270;
						}
					}
				}
				if (oval<0)
					oval+=360;
				state = end;
				break;
			case end:
				break;
		}
	}
	return oval;
}
