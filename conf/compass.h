#ifndef COMPASS_H
#define COMPASS_H

#include "hal.h"
#include "GLOBALES.h"
#include <math.h>
#include <string.h>

#define MB_COMPASS_MSG_SIZE 			1

#define HMC_I2C_ADDRESS						0x1E

// Registers
// Configuration Register A
#define HMC_CONF_REGISTER_A				0x00	// r/w
#define HMC_CONF_REGISTER_B				0x01	// r/w
#define HMC_MODE_REGISTER					0x02	// r/w
#define HMC_DATA_OUTPUT_X_MSB			0x03	// r
#define HMC_DATA_OUTPUT_X_LSB			0x04	// r
#define HMC_DATA_OUTPUT_Y_MSB			0x07	// r
#define HMC_DATA_OUTPUT_Y_LSB			0x08	// r
#define HMC_DATA_OUTPUT_Z_MSB			0x05	// r
#define HMC_DATA_OUTPUT_Z_LSB			0x06	// r
#define HMC_STATUS_REGISTER				0x09	// r
#define HMC_ID_REGISTER_A					0x10	// r
#define HMC_ID_REGISTER_B					0x11	// r
#define HMC_ID_REGISTER_C					0x12	// r

// HMC_CONF_REGISTER_A:
#define HMC_NUM_1_SAMPLES					0x00	// 1 sample (default)
#define HMC_NUM_2_SAMPLES					0x20	// 2 samples
#define HMC_NUM_4_SAMPLES					0x40	// 4 samples
#define HMC_NUM_8_SAMPLES					0x60	// 8 samples

#define HMC_OUTPUT_RATE_O_75Hz		0x00	// 0.75 Hz
#define HMC_OUTPUT_RATE_1_5Hz			0x04	// 1.5 Hz
#define HMC_OUTPUT_RATE_3Hz				0x08	// 3 Hz
#define HMC_OUTPUT_RATE_7_5Hz			0x0C	// 7.5 Hz
#define HMC_OUTPUT_RATE_15Hz			0x10	// 15 Hz (default)
#define HMC_OUTPUT_RATE_30Hz			0x14	// 30 Hz
#define HMC_OUTPUT_RATE_75Hz			0x1C	// 75 Hz

#define HMC_NORMAL_MEASUR_MODE		0x00	// Left floating and high impedance (default)
#define HMC_POS_BIAS_MEASUR_MODE	0x01	// Positive current is forced accross the resistive load
#define HMC_NEG_BIAS_MEASUR_MODE	0x02	// Negative current is forced accross the resistive load

// HMC_CONF_REGISTER_B:
#define HMC_GAIN_CONF_1370				0x00	// Gain: 1370LSb/Gauss, Resolution: 0.73mG/LSb
#define HMC_GAIN_CONF_1090				0x20	// Gain: 1090LSb/Gauss, Resolution: 0.92mG/LSb (default)
#define HMC_GAIN_CONF_820					0x40	// Gain: 820LSb/Gauss, Resolution: 1.22mG/LSb
#define HMC_GAIN_CONF_660					0x60	// Gain: 660LSb/Gauss, Resolution: 1.52mG/LSb
#define HMC_GAIN_CONF_440					0x80	// Gain: 440LSb/Gauss, Resolution: 2.27mG/LSb
#define HMC_GAIN_CONF_390					0xA0	// Gain: 390LSb/Gauss, Resolution: 2.56mG/LSb
#define HMC_GAIN_CONF_330					0xC0	// Gain: 330LSb/Gauss, Resolution: 3.03mG/LSb
#define HMC_GAIN_CONF_230					0xE0	// Gain: 230LSb/Gauss, Resolution: 4.35mG/LSb

// HMC_MODE_REGISTER:
#define HMC_I2C_HIGH_SPEED				0x80	// Set I2C to 3400kHz
#define HMC_I2C_NORMAL_SPEED			0x00	// Set I2C to normal speed (default)

#define HMC_CONTINUOUS_MEAS_MODE	0x00
#define HMC_SINGLE_MEAS_MODE			0x01

// HMC_STATUS_REGISTER:
#define HMC_DATA_OUTPUT_LOCK			0x02
#define HMC_DATA_OUTPUT_READY			0x01


// Task prototype
static WORKING_AREA(waCompass, 256);
msg_t ThreadCompass( void *arg );

// Functions prototypes
void initCompass( void );
int16_t getAngle( void );

#endif //COMPASS_H
