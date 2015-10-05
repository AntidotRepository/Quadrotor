#ifndef ALTI_H
#define ALTI_H

#include "math.h"
#include "hal.h"
#include "GLOBALES.h"
#include "alti.h"

typedef struct PressureVar PressureVar;
struct PressureVar
{
	long X1;
	long X2;
	long X3;

	long B3;
	unsigned long B4;
	long B5;
	long B6;
	unsigned long B7;
};

typedef struct BMP085_reg BMP085_reg;
struct BMP085_reg
{
	uint8_t ac1[2]; //Registre 0xAA
	uint8_t ac2[2]; //Registre 0xAC
	uint8_t ac3[2]; //Registre 0xAE
	uint8_t ac4[2]; //Registre 0xB0
	uint8_t ac5[2]; //Registre 0xB2
	uint8_t ac6[2]; //Registre 0xB4

	uint8_t b1[2]; //Registre 0xB6
	uint8_t b2[2]; //Registre 0xB8

	uint8_t mb[2]; //Registre 0xBA
	uint8_t mc[2]; //Registre 0xBC
	uint8_t md[2]; //Registre 0xBE

};

typedef struct long_BMP085_reg long_BMP085_reg;
struct long_BMP085_reg
{
	int16_t ac1; //Registre 0xAA
	int16_t ac2; //Registre 0xAC
	int16_t ac3; //Registre 0xAE
	uint16_t ac4; //Registre 0xB0
	uint16_t ac5; //Registre 0xB2
	uint16_t ac6; //Registre 0xB4

	int16_t b1; //Registre 0xB6
	int16_t b2; //Registre 0xB8

	int16_t mb; //Registre 0xBA
	int16_t mc; //Registre 0xBC
	int16_t md; //Registre 0xBE
};

#define MB_ALTI_MSG_SIZE 1

// Task prototype
static WORKING_AREA(waAlti, 512);
msg_t ThreadAlti( void *arg );

// Functions prototypes
long readUncompensatedTemperature( void );
long readUncompensatedPressure( void );
long calculateTemperature(void);
double calculatePressure(void);
void initAlti( void );
float getAltitude( void );

#endif //ALTI_H
