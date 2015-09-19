#include "gyro.h"

extern Mailbox mb_gyro;
extern msg_t mb_gyro_buf[MB_GYRO_MSG_SIZE];

msg_t ThreadGyro( void *arg )
{
	msg_t msg;
	DATA_GYRO data;
	systime_t time = chTimeNow();
	int refreshRate = 1000/GYRO_REFRESH_RATE;
	
	initGyro();
	
	while(TRUE)
	{
		time += MS2ST(refreshRate);
		
		data.angleRoulis = getAngle(MPU_ACCEL_XOUT_H_ADDR);
		data.angleTangage = getAngle(MPU_ACCEL_YOUT_H_ADDR);
		data.angleLacet = getAngle(MPU_ACCEL_ZOUT_H_ADDR);
		data.accRoulis = getAcceleration(MPU_ACCEL_XOUT_H_ADDR);
		data.accTangage =  getAcceleration(MPU_ACCEL_YOUT_H_ADDR);
		data.accLacet = getAcceleration(MPU_ACCEL_ZOUT_H_ADDR);
		
		msg = (msg_t)&data;
		chMBPost(&mb_gyro, msg, TIME_IMMEDIATE);
		chThdSleepUntil(time);
	}
}

void initGyro()
{
	uint8_t txbuf[2] = {0};
	
	// MPU initialization
	txbuf[0] = MPU_SMPLRT_DIV_ADDR;
	txbuf[1] = MPU_SMPLRT_DIV_1000Hz;
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, MPU_I2C_ADDRESS, txbuf, 2, NULL, 0, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);

	txbuf[0] = MPU_CONFIG_ADDR;
	txbuf[1] = EXT_SYNC_SET_DISABLE | DLPF_CFG_260Hz;
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, MPU_I2C_ADDRESS, txbuf, 2, NULL, 0, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = MPU_GYRO_CONFIG_ADDR;
	txbuf[1] = FS_SEL_500;
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, MPU_I2C_ADDRESS, txbuf, 2, NULL, 0, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = MPU_ACCEL_CONFIG_ADDR;
	txbuf[1] = FS_SEL_2G;
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, MPU_I2C_ADDRESS, txbuf, 2, NULL, 0, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = MPU_PWR_MGMT_1_ADDR;
	txbuf[1] = MPU_ENABLE | PLL_Y_GYROSCOPE | INTERNAL_8MHZ_OSCILLATOR | TEMP_SENS_ENABLE;																												//Donnée																											//Donnée
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, MPU_I2C_ADDRESS, txbuf, 2, NULL, 0, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
}

float getAngle( int axis )
{
	float m_forceVector;
	float res = 0;
	
	int16_t angle = 0;
	
	uint8_t txbuf[10] = {0};
	uint8_t rxbuf[10] = {0};

	txbuf[0] = axis;
	
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 1, rxbuf, 2, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	angle = rxbuf[0]<<8;
	angle = angle | (unsigned int)rxbuf[1];
	
	
	// calculer vecteur de force
	m_forceVector = sqrt( pow(getAcceleration(MPU_ACCEL_XOUT_H_ADDR), 2 )
											+ pow(getAcceleration(MPU_ACCEL_YOUT_H_ADDR), 2 )
											+ pow(getAcceleration(MPU_ACCEL_ZOUT_H_ADDR), 2 ));
	
	res = acos(getAcceleration(axis)/m_forceVector)*57.295779506;
	return res;
}

double getAcceleration( int axis )
{
	int16_t accel = 0;
		
	uint8_t txbuf[2] = {0};
	uint8_t rxbuf[2] = {0};
	
	txbuf[0] = axis;
	
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 1, rxbuf, 2, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	accel = rxbuf[0]<<8;
	accel = accel | (unsigned int)rxbuf[1];
	
	return accel/2;
}
