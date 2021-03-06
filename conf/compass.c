#include "compass.h"

extern Mailbox mb_compass;
extern msg_t mb_compass_buf[MB_COMPASS_MSG_SIZE];

enum states
{
	check,
	get,
	calibration,
	calculation,
	end
};


msg_t ThreadCompass( void *arg )
{
	msg_t msg;
	float angle = 0;
	systime_t time = chTimeNow();
	int refreshRate = 1000/COMPASS_REFRESH_RATE;
	
	initCompass();
	
	while(TRUE)
	{
		time += MS2ST(refreshRate);
		
		angle = getNorth();
		if(angle >= 0 && angle <=360)
		{
			msg = (msg_t)&angle;
			chMBPost(&mb_compass, msg, TIME_IMMEDIATE);
			
			if(time < chTimeNow())
			{
				time = chTimeNow();
			}
			chThdSleepUntil(time);
		}
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

float getNorth()
{
	#ifdef DEBUG_WARNINGS
	#warning 1 - For the moment, the north won't be given accurately when the compass (so the quadcopter) will tilt: Need to be done
	#warning 2 - Should be better to add something to compensate the magnetic declination (discussed here: http://bildr.org/2012/02/hmc5883l_arduino/)
	#endif
	int state = check;
	uint8_t txbuf[2] = {0};
	uint8_t rxbuf[6] = {0};
	
	int16_t rawX = 0;
	int16_t rawY = 0;
	int16_t rawZ = 0;
	int16_t ArawX = fabs(rawX);
	int16_t ArawY = fabs(rawY);
	int16_t ArawZ = fabs(rawZ);
	
	static int16_t maxRawX = 0;
	static int16_t minRawX = 0;
	static int16_t maxRawY = 0;
	static int16_t minRawY = 0;
	static int16_t maxRawZ = 0;
	static int16_t minRawZ = 0;
	
	int16_t X_offset = 0;
	int16_t Y_offset = 0;
	int16_t Z_offset = 0;
	
	float average = 0;
	float X_scale = 0;
//	float Y_scale = 0;
//	float Z_scale = 0;
	
	int8_t quad = 0;
	float ival, oval, aival;
	
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
				rawZ = rxbuf[2]<<8 | rxbuf[3];
			
				ArawX = fabs(rawX);												// We get absolute value of rawX
				ArawY = fabs(rawY);												// We get absolute value of rawY
				ArawZ = fabs(rawZ);
			
				state = calibration;
				break;
			
			case calibration:
				/*
					Code to compensate magnetic distortion
					http://theboredengineers.com/tag/hmc5883l/
				*/
				if(rawX < minRawX)
					minRawX = rawX;
				else if(rawX > maxRawX)
					maxRawX = rawX;
				if(rawY < minRawY)
					minRawY = rawY;
				else if(rawY > maxRawY)
					maxRawY = rawY;
				if(rawZ < minRawZ)
					minRawZ = rawZ;
				else if(rawZ > maxRawZ)
					maxRawZ = rawZ;
				
				X_offset = (maxRawX+minRawX)/2;
				Y_offset = (maxRawY+minRawY)/2;
				Z_offset = (maxRawZ+minRawZ)/2;
				
				average = (float)(maxRawX+maxRawY+maxRawZ)/3;
				X_scale = (float)maxRawX/average;
				//Y_scale = (float)maxRawY/average;
				//Z_scale = (float)maxRawZ/average;
				
				rawX = X_scale*(rawX - X_offset);
				rawY = X_scale*(rawY - Y_offset);
				rawZ = X_scale*(rawZ - Z_offset);
				
				state = calculation;
				break;
				
			case calculation:
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
				if(oval > 0 && oval < 360)
					state = end;
				else
					state = check;
				break;
			
			case end:
				break;
		}
	}
	return oval;
}
