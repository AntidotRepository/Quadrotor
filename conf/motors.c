#include "motors.h"

static void motor1up(PWMDriver *pwmp)
{
	(void)pwmp;
	palSetPad(GPIOB, GPIOB_PIN15);
}

static void motor2up(PWMDriver *pwmp)
{
	(void)pwmp;
	palSetPad(GPIOB, GPIOB_PIN14);
}

static void motor3up(PWMDriver *pwmp)
{
	(void)pwmp;
	palSetPad(GPIOB, GPIOB_PIN13);
}

static void motor4up(PWMDriver *pwmp)
{
	(void)pwmp;
	palSetPad(GPIOB, GPIOB_PIN12);
}

static void motorDown(PWMDriver *pwmp)
{
	(void)pwmp;
	palClearPad(GPIOB, GPIOB_PIN15);
	palClearPad(GPIOB, GPIOB_PIN14);
	palClearPad(GPIOB, GPIOB_PIN13);
	palClearPad(GPIOB, GPIOB_PIN12);
}

static PWMConfig pwmMotor = 
{
	PWM_CLK_FREQUENCY,				// 10khz PWM clock frequency
	TICKS_CNT_PSC,						// number of ticks for a period
	motorDown,
	{
		{ PWM_OUTPUT_ACTIVE_HIGH, motor1up },
		{ PWM_OUTPUT_ACTIVE_HIGH, motor2up },
		{ PWM_OUTPUT_ACTIVE_HIGH, motor3up },
		{ PWM_OUTPUT_ACTIVE_HIGH, motor4up }
	},
	0,
	0
};

void initMotor()
{
	palSetPadMode(GPIOB, GPIOB_PIN12, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN13, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN14, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN15, PAL_MODE_OUTPUT_PUSHPULL);
	
	pwmStart(&PWMD2, &pwmMotor);
	
	// initialization sequence
	setSpeed(0, MOTOR_1);
	setSpeed(0, MOTOR_2);
	setSpeed(0, MOTOR_3);
	setSpeed(0, MOTOR_4);
	
	chThdSleepMilliseconds(3000);
	setSpeed(100, MOTOR_1);
	setSpeed(100, MOTOR_2);
	setSpeed(100, MOTOR_3);
	setSpeed(100, MOTOR_4);
	
	chThdSleepMilliseconds(3000);
}

void setSpeed(int speed, int motor)
{	
	pwmEnableChannel(&PWMD2, motor, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, (speed/2+50)*100));
}

