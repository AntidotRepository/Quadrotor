#ifndef GLOBALES_H
#define GLOBALES_H

// Uncomment this line to show my own warnings
//#define DEBUG_WARNINGS

// ------------------------------------
// -------------	Motors  -------------
// ------------------------------------
	#define MOTORS_REFRESH_RATE		STABI_REFRESH_RATE
	#define MOTOR_1								0
	#define MOTOR_2								1
	#define MOTOR_3								2
	#define MOTOR_4								3
	
	#define PWM_CLK_FREQUENCY 		10000
	#define TICKS_CNT_PSC					20
	
	
// ------------------------------------
// -------------	  I²C   -------------
// ------------------------------------
	#define I2C_CLK_SPEED					320000 // A ne pas changer, sinon erreur! 
	#define I2C_TIMEOUT						1000

// ------------------------------------
// -----------	  Gyroscop   ----------
// ------------------------------------
	#define AXIS_X								0
	#define AXIS_Y								1
	#define AXIS_Z								2
	
	#define GYRO_REFRESH_RATE			70

// ------------------------------------
// ----------	  altimeter   -----------
// ------------------------------------
	#define OSS 3 //Oversampling settings (taux d'échantillonage du BMP085 compris entre 0 (faible taux mais peu énergivore) et 3 (8 échantillons avant envois mais très énergivore))
	#define ALTI_REFRESH_RATE			2					// Hz
		
// ------------------------------------
// ----------  Communication ----------
// ------------------------------------
	#define XBEE_REFRESH_RATE			10				// Hz
	#define BAUDRATE							115200		

// ------------------------------------
// -----------   Compass   ------------
// ------------------------------------
	#define COMPASS_REFRESH_RATE	10				// Hz

// ------------------------------------
// ---------  Stabilization   ---------
// ------------------------------------
	#define STABI_REFRESH_RATE		10				// Hz

#endif // GLOBALES_H


