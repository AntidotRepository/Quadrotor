#ifndef GYRO_H
#define GYRO_H

#include "hal.h"
#include "math.h"
#include "GLOBALES.h"

typedef struct DATA_GYRO DATA_GYRO;
struct DATA_GYRO
{
	int16_t 	angleTangage;
	int16_t 	angleRoulis;
	int16_t		angleLacet;
	double 		accTangage;
	double		accRoulis;
	double		accLacet;
};

#define MB_MSG_SIZE 1

// Task prototype
static WORKING_AREA(waGyro, 512);
msg_t ThreadGyro( void *arg );
	
// Functions prototypes
void initGyro( void );
int16_t getAngle( int axis );
double getAcceleration( int axis );

#endif // GYRO_H
