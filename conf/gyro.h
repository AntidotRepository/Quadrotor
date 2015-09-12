#ifndef GYRO_H
#define GYRO_H

#include "hal.h"
#include "math.h"
#include "GLOBALES.h"

#define MB_GYRO_MSG_SIZE 							1

#define MPU_I2C_ADDRESS								0x69

// Registers
#define MPU_SELF_TEST_X_ADDR					0x0D	// r/w
#define MPU_SELF_TEST_Y_ADDR					0x0E	// r/w
#define MPU_SELF_TEST_Z_ADDR					0x0F	// r/w
#define MPU_SELF_TEST_A_ADDR					0x10	// r/w
#define MPU_SMPLRT_DIV_ADDR						0x19	// r/w
#define MPU_CONFIG_ADDR								0x1A	// r/w
#define MPU_GYRO_CONFIG_ADDR					0x1B	// r/w
#define MPU_ACCEL_CONFIG_ADDR					0x1C	// r/w
#define MPU_FIFO_EN_ADDR							0x23	// r/w
#define MPU_I2C_MST_CTRL_ADDR					0x24	// r/w
#define MPU_I2C_SLV0_ADDR_ADDR				0x25	// r/w
#define MPU_I2C_SLV0_REG_ADDR					0x26	// r/w
#define MPU_I2C_SLV0_CTRL_ADDR				0x27	// r/w
#define MPU_I2C_SLV1_ADDR_ADDR				0x28	// r/w
#define MPU_I2C_SLV1_REG_ADDR					0x29	// r/w
#define MPU_I2C_SLV1_CTRL_ADDR				0x2A	// r/w
#define MPU_I2C_SLV2_ADDR_ADDR				0x2B	// r/w
#define MPU_I2C_SLV2_REG_ADDR					0x2C	// r/w
#define MPU_I2C_SLV2_CTRL_ADDR				0x2D	// r/w
#define MPU_I2C_SLV3_ADDR_ADDR				0x2E	// r/w
#define MPU_I2C_SLV3_REG_ADDR					0x2F	// r/w
#define MPU_I2C_SLV3_CTRL_ADDR				0x30	// r/w
#define MPU_I2C_SLV4_ADDR_ADDR				0x31	// r/w
#define MPU_I2C_SLV4_REG_ADDR					0x32	// r/w
#define MPU_I2C_SLV4_DO_ADDR					0x33	// r/w
#define MPU_I2C_SLV4_CTRL_ADDR				0x34	// r/w
#define MPU_I2C_SLV4_DI_ADDR					0x35	// r
#define MPU_I2C_MST_STATUS_ADDR				0x36	// r
#define MPU_INT_PIN_CFG_ADDR					0x37	// r/w
#define MPU_INT_ENABLE_ADDR						0x38	// r/w
#define MPU_INT_STATUS_ADDR						0x3A	// r
#define MPU_ACCEL_XOUT_H_ADDR					0x3B	// r
#define MPU_ACCEL_XOUT_L_ADDR					0x3C	// r
#define MPU_ACCEL_YOUT_H_ADDR					0x3D	// r
#define MPU_ACCEL_YOUT_L_ADDR					0x3E	// r
#define MPU_ACCEL_ZOUT_H_ADDR					0x3F	// r
#define MPU_ACCEL_ZOUT_L_ADDR					0x40	// r
#define MPU_TEMP_OUT_H_ADDR						0x41	// r
#define MPU_TEMP_OUT_L_ADDR						0x42	// r
#define MPU_GYRO_XOUT_H_ADDR					0x43	// r
#define MPU_GYRO_XOUT_L_ADDR					0x44	// r
#define MPU_GYRO_YOUT_H_ADDR					0x45	// r
#define MPU_GYRO_YOUT_L_ADDR					0x46	// r
#define MPU_GYRO_ZOUT_H_ADDR					0x47	// r
#define MPU_GYRO_ZOUT_L_ADDR					0x48	// r
// ... EXT_SENS_DATA_[0..23] ...
#define MPU_I2C_SLV0_DO_ADDR					0x63	// r/w
#define MPU_I2C_SLV1_DO_ADDR					0x64	// r/w
#define MPU_I2C_SLV2_DO_ADDR					0x65	// r/w
#define MPU_I2C_SLV3_DO_ADDR					0x66	// r/w
#define MPU_I2C_MST_DELAY_CTRL_ADDR		0x67	// r/w
#define MPU_SIGNAL_PATH_RESET_ADDR		0x68	// r/w
#define MPU_USER_CTRL_ADDR						0x6A	// r/w
#define MPU_PWR_MGMT_1_ADDR						0x6B	// r/w
#define MPU_PWR_MGMT_2_ADDR						0x6C	// r/w
#define MPU_FIFO_COUNTH_ADDR					0x72	// r/w
#define MPU_FIFO_COUNTL_ADDR					0x73	// r/w
#define MPU_FIFO_R_W_ADDR							0x74	// r/w
#define MPU_WHO_AM_I_ADDR							0x75	// r

// MPU_SMPLRT_DIV_ADDR
#define MPU_SMPLRT_DIV_1000Hz					7		// (1000/(MPU_SMPLRT_DIV+1)) = 400Hz

// MPU_CONFIG_ADDR
#define EXT_SYNC_SET_DISABLE					0x00
#define EXT_SYNC_SET_TEMP_OUT_L				0x08
#define EXT_SYNC_SET_GYRO_XOUT_L			0x10
#define EXT_SYNC_SET_GYRO_YOUT_L			0x18
#define EXT_SYNC_SET_GYRO_ZOUT_L			0x20
#define EXT_SYNC_SET_ACCEL_XOUT_L			0x28
#define EXT_SYNC_SET_ACCEL_YOUT_L			0x30
#define EXT_SYNC_SET_ACCEL_ZOUT_L			0x38
#define DLPF_CFG_260Hz								0x00
#define DLPF_CFG_184Hz								0x01
#define DLPF_CFG_94Hz									0x02
#define DLPF_CFG_44Hz									0x03
#define DLPF_CFG_21Hz									0x04
#define DLPF_CFG_10Hz									0x05
#define DLPF_CFG_5Hz									0x06

// MPU_GYRO_CONFIG_ADDR
#define FS_SEL_250										0x00
#define FS_SEL_250_DIVIDER						131
#define FS_SEL_500										0x08
#define FS_SEL_500_DIVIDER						66.5
#define FS_SEL_1000										0x10
#define FS_SEL_1000_DIVIDER						32.8
#define FS_SEL_2000										0x18
#define FS_SEL_2000_DIVIDER						16.4

// MPU_ACCEL_CONFIG_ADDR
#define FS_SEL_2G											0x00
#define FS_SEL_4G											0x08
#define FS_SEL_8G											0x10
#define FS_SEL_16G										0x18

// MPU_PWR_MGMT_1_ADDR
#define MPU_ENABLE										0x00
#define MPU_SLEEP											0x40
#define MPU_CYCLING										0x20
#define MPU_NO_CYCLING								0x20
#define INTERNAL_8MHZ_OSCILLATOR			0x00
#define PLL_X_GYROSCOPE								0x01
#define PLL_Y_GYROSCOPE								0x02
#define PLL_Z_GYROSCOPE								0x03
#define PLL_32768Khz_EXT_REF					0x04
#define PLL_19_2MHZ_EXT_REF						0x05
#define TEMP_SENS_DISABLE							0x08
#define TEMP_SENS_ENABLE							0x00


typedef struct DATA_GYRO DATA_GYRO;
struct DATA_GYRO
{
	float 		angleTangage;
	float		 	angleRoulis;
	float			angleLacet;
	double 		accTangage;
	double		accRoulis;
	double		accLacet;
};

// Task prototype
static WORKING_AREA(waGyro, 512);
msg_t ThreadGyro( void *arg );
	
// Functions prototypes
void initGyro( void );
float getAngle( int axis );
double getAcceleration( int axis );

#endif // GYRO_H
