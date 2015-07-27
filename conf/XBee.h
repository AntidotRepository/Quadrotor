/*

Exemple d'utilisation:
	
	 SerialConfig uartCfg=
{
	 BAUDRATE,									// Baudrate
	 0,											 		// cr1 register values
	 0,											 		// cr2 register values
	 0													// cr3 register values
};


	DATA_COMM data;
	
	data.altitude = 12345;
	data.battery = 95.26;
	data.lacet = 123;
	data.roulis = 456;
	data.signal = 51.01;
	data.tangage = 15;
	
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
	
	sdStart(&SD2, &uartCfg);
	
	sendData(&data);
	
*/

#ifndef XBEE_H
#define XBEE_H

#include "stdio.h"
#include "hal.h"
#include "string.h"
#include "GLOBALES.h"

typedef struct DATA_COMM DATA_COMM;
struct DATA_COMM
{
	int 	tangage;
	int 	roulis;
	int 	lacet;
	int 	altitude;
	int 	battery;
	int 	signal;
};

// Task prototype
msg_t ThreadComSnd( void *arg );

// Functions prototypes
void initXBee( void );
void sendData (DATA_COMM *data );
DATA_COMM rcvData( void );
float getSignal( void );

#endif //XBEE_H
