#ifndef STABI_H
#define STABI_F

#include "GLOBALES.h"
#include "math.h"
#include "deriv.h"
#include "hal.h"
#include "motors.h"

#define MB_STABI_MSG_SIZE 1
#define MB_STABI_MOTOR_MSG_SIZE 1

// Task prototype
static WORKING_AREA(waStabi, 512);
msg_t ThreadStabi( void *arg );

typedef struct DATA_STABI DATA_STABI;
struct DATA_STABI
{
	float	roll; 		//rad
	float	dotRoll; 	//rad/s
	float	pitch;      //rad
	float	dotPitch;	//rad/s
	float	yaw;		//rad
	float	dotYaw;		//rad/s
	float	posZ;		//m
	float	dotPosZ;	//m/s
	float	posX;		//m
	float	dotPosX;		//m/s
	float	posY;		//m
	float	dotPosY;	//m/s
};

typedef struct U_struc U_struc;
struct U_struc
{
	float U1;
	float U2;
	float U3;
	float U4;
};

extern float sp; //(s) sampling period

extern float kpz;
extern float kdz;
extern float kpp;
extern float kdp;
extern float kpt;
extern float kdt;
extern float kpps;
extern float kdps;

extern float m; // (kg)
extern float g; //(m/s^2)

//***** constants of the linear curve Omega=f(bin) *****
extern float slo; // slope (of the linear curve om=f(bin))
extern float shi;  // shift

// alt_control
float alt_control(DATA_STABI state_quadri,float d_posZ);

// spd_control
void spd_control(DATA_STABI *state_quadri, DATA_STABI *state_quadri_d, float poussee);

// PDController
U_struc PDController(DATA_STABI *state_quadri, DATA_STABI *state_quadri_d);

// motors
U_struc U2bin(U_struc U);

#endif//STABI_H
