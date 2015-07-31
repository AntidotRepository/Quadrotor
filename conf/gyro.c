#include "gyro.h"

extern Mailbox mb_gyro;
extern msg_t mb_gyro_buf[MB_MSG_SIZE];

msg_t ThreadGyro( void *arg )
{
	msg_t msg;
	
	DATA_GYRO data;
	initGyro();
	
	while(TRUE)
	{		
		// We get informations from sensor and store in it
		data.angleRoulis = getAngle(REG_GYRO_X);
		data.angleTangage = getAngle(REG_GYRO_Y);
		data.angleLacet = getAngle(REG_GYRO_Z);
		data.accRoulis = getAcceleration(REG_ACCEL_X);
		data.accTangage =  getAcceleration(REG_ACCEL_Y);
		data.accLacet = getAcceleration(REG_ACCEL_Z);
		msg = (msg_t)&data;
		chMBPost(&mb_gyro, msg, TIME_IMMEDIATE);
		
		chThdSleepMilliseconds(2.5); //Refresh rate @400Hz
	}
}

void initGyro()
{
	//tableau stockage données envoyées sur l'I²C
	uint8_t txbuf[10] = {0};

	//Initialisation du gyroscope
	txbuf[0] = 0x6B;																												//Registre
	txbuf[1] = 0x00;																												//Donnée
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x19;// registre fréquence échantillonage										//Registre
	txbuf[1] = 0x14;// Echantillonage à 400Hz																//Donnée
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Réglage de la fréquence d'échantillonage (125Hz)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1A;																												//Registre
	txbuf[1] = 0x06;																												//Donnée
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Réglage de la fréquence du filtre passe-bas (5Hz)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1B;																												//Registre
	txbuf[1] = 0x18;																												//Donnée
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Soi et la plage de mesure du gyroscope, des valeurs typiques: 0x18 (pas d'auto, 2000deg / s)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1C;																												//Registre
	txbuf[1] = 0x00;																												//Donnée
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Gamme d'accéléromètre d'auto-test et mesure la fréquence du filtre passe-haut, les valeurs typiques: 0x01 (non auto, 2G, 5 Hz)
	i2cReleaseBus(&I2CD1);
}

int16_t getAngle( int axis )
{
	double m_forceVector;
	
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
	m_forceVector = sqrt( pow(getAcceleration(REG_ACCEL_X), 2 )
											+ pow(getAcceleration(REG_ACCEL_Y), 2 )
											+ pow(getAcceleration(REG_ACCEL_Z), 2 ));
	return acos(getAcceleration(axis)/m_forceVector)*57.295779506;
}

double getAcceleration( int axis )
{
	int16_t accel = 0;
		
	uint8_t txbuf[10] = {0};
	uint8_t rxbuf[10] = {0};
	
	txbuf[0] = axis;
	
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 1, rxbuf, 2, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	accel = rxbuf[0]<<8;
	accel = accel | (unsigned int)rxbuf[1];
	
	return accel/2;
}
