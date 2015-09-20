#ifndef GLOBALES_H
#define GLOBALES_H



// ------------------------------------
// -------------	Motors  -------------
// ------------------------------------
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
	#define I2C_TIMEOUT						5000

// ------------------------------------
// -----------	  Gyroscop   ----------
// ------------------------------------
	#define AXIS_X								0
	#define AXIS_Y								1
	#define AXIS_Z								2
	
	#define GYRO_REFRESH_RATE			100

// ------------------------------------
// ----------	  altimeter   -----------
// ------------------------------------
	#define OSS 3 //Oversampling settings (taux d'échantillonage du BMP085 compris entre 0 (faible taux mais peu énergivore) et 3 (8 échantillons avant envois mais très énergivore))
	
// ------------------------------------
// ----------  Communication ----------
// ------------------------------------
	#define XBEE_REFRESH_RATE			20				// Hz
	#define BAUDRATE							115200		

// ------------------------------------
// -----------   Compass   ------------
// ------------------------------------
	#define COMPASS_REFRESH_RATE	100				// Hz

#endif // GLOBALES_H
