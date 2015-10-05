#include "stabi.h"

extern Mailbox mb_stabi_state;
extern Mailbox mb_stabi_state_d;
extern Mailbox mv_stabi_motor;

extern msg_t mb_alti_buf[MB_STABI_MSG_SIZE];

float kpz = 100;    // Height P controller
float kdz = 20;     // Height D controller
float kpp = 0.8;    // Roll P controller
float kdp = 0.4;    // Roll D controller
float kpt = 1.2;    // Pitch P controller
float kdt = 0.4;    // Pitch D controller
float kpps = 1;     // Yaw P controller
float kdps = 0.4;   // Yaw D controller

// ---- GLOBALES VARIABLES ------
float m = 1.0;      // Quadcopter mass
float g = 9.806;    // gravity

float sp = 0.01;    // sampling period

float b = 0.08;     // Thrust factor
float d = 3.33;     // Drag factor

//***** constants of the linear curve Omega=f(bin) *****
float slo=2.7542; // slope (of the linear curve om=f(bin))
float shi=3.627;  // shift

msg_t ThreadStabi( void *arg )
{
	float poussee           = 0;										// Poussee à appliquer [N]
	msg_t msg, msg_d;
	systime_t time = chTimeNow();
	int refreshRate = 1000/STABI_REFRESH_RATE;
	
	DATA_STABI *state_quadri      = {0};										// position réelle du quadri
  DATA_STABI *state_quadri_d    = {0};                                      // position désirée du quadri

	U_struc U1, U2;
	
	// For tests only!
	state_quadri->roll       = 0; // [rad]
	state_quadri->dotRoll    = 0; // [rad/s]
	state_quadri->pitch      = 0; // [rad]
	state_quadri->dotPitch   = 0; // [rad/s]
	state_quadri->yaw        = 0; // [rad]
	state_quadri->dotYaw     = 0; // [rad/s]
	state_quadri->posX       = 0; // [m]
	state_quadri->dotPosX    = 0; // [m/s]
	state_quadri->posY       = 0; // [m]
	state_quadri->dotPosY    = 0; // [m/s]
	state_quadri->posZ       = 0; // [m]
	state_quadri->dotPosZ    = 0; // [m/s]

	state_quadri_d->roll     = 0; // [rad]
	state_quadri_d->dotRoll  = 0; // [rad/s]
	state_quadri_d->pitch    = 0; // [rad]
	state_quadri_d->dotPitch = 0; // [rad/s]
	state_quadri_d->yaw      = 0; // [rad]
	state_quadri_d->dotYaw   = 0; // [rad/s]
	state_quadri_d->posX     = 0; // [m]
	state_quadri_d->dotPosX  = 0; // [m/s]
	state_quadri_d->posY     = 0; // [m]
	state_quadri_d->dotPosY  = 0; // [m/s]
	state_quadri_d->posZ     = -1; // [m]
	state_quadri_d->dotPosZ  = 0; // [m/s]
#ifdef DEBUG_WARNINGS
#warning le _d.posZ doit être en relatif par rapport à la posZ réelle. Donc, si je veux monter de 10m, je dois mettre -10!
#endif
	
	initMotor();
	
	while(1)
	{
		time += MS2ST(refreshRate);
		
		if( state_quadri->yaw != state_quadri->yaw)
			__NOP();

		// Get data from the mailboxes
		chMBFetch(&mb_stabi_state, &msg, TIME_INFINITE);
		state_quadri =(DATA_STABI*)msg;
/*		chMBFetch(&mb_stabi_state_d, &msg_d, TIME_INFINITE);
		state_quadri_d = (DATA_STABI*)msg_d;
*/	
		poussee = alt_control(
								*state_quadri,
								state_quadri_d->posZ
							);
		
		spd_control(
						state_quadri,
						state_quadri_d,
						poussee
					);
		
		U1 = PDController(
						state_quadri,
						state_quadri_d
					);
								
		U2 = U2bin(
						U1
					);
					
		// 100% = 16000rpm (need to be check)
//		#error J en suis à: compiler/flasher tester sortie avec Oscillo puis avec moteurs
		
		setSpeed((U2.U1)/160, MOTOR_1);
		setSpeed((U2.U2)/160, MOTOR_2);
		setSpeed((U2.U3)/160, MOTOR_3);
		setSpeed((U2.U4)/160, MOTOR_4);
		
		if(time < chTimeNow())
		{
			time = chTimeNow();
		}
		chThdSleepUntil(time);
	}
}

float alt_control(DATA_STABI state_quadri, float d_posZ)
{
	//short a1 = 2; -> check if really usefull (seems not)
	short a2 = 4;

	float r1 = -a2*(state_quadri.posZ-d_posZ);
	float poussee = (r1+m*g)/(cos(state_quadri.roll)*cos(state_quadri.pitch));
    //printf("poussee = %lf\n", poussee);
	return poussee;
}

/**
*	spd_control()
*	Permet de calculer l'angle de tangage et de roulis
*	à appliquer pour atteindre une vitesse donnée.
*	entrée:
*		- state_quadri (DATA_STABI): position du quadricoptère
*		- d_dotPosX (float): vitesse désirée du quadri en X (m/s)
*		- d_dotPosY (float): vitesse désirée du quadri en Y (m/s)
*		- poussee (float): poussée du quadri (calculée par alt_control, donné en Newton)
*	sorties:
*		- rolldPitchd (DATA_STABI):
*			- roll (float): roulis à appliquer (rad)
*			- pitch (float): tangage à appliquer (rad)
*/
void spd_control(DATA_STABI *state_quadri, DATA_STABI *state_quadri_d, float poussee)
{
	float UX = 0;
	float UY = 0;

	float rolld = 0;
	float pitchd = 0;

	int ax = 1;
	int ay = 1;

	float ex = state_quadri_d->dotPosX - state_quadri->dotPosX;
	float ey = state_quadri_d->dotPosY - state_quadri->dotPosY;

	if(poussee > 0)
	{
		UX = (ax*ex)*m/poussee;
		UY = (ay*ey)*m/poussee;

		rolld = asin(UX*sin(state_quadri->yaw) - UY*cos(state_quadri->yaw));
		pitchd = asin((UX-sin(state_quadri->roll)*sin(state_quadri->yaw))/(cos(state_quadri->roll)*cos(state_quadri->yaw)));
	}
	else
	{
		poussee = m*g;
		rolld = 0;
		pitchd = 0;
	}
	//printf("rolld = %lf\npitchd = %lf\n", rolld, pitchd);
	state_quadri_d->roll = rolld;
	state_quadri_d->pitch = pitchd;
}

static DATA_STABI old_state_quadri = {0};

U_struc PDController(DATA_STABI *state_quadri, DATA_STABI *state_quadri_d)
{
    U_struc U = {0};

    //printf("posZ = %lf, posZd = %lf, oldPosZ = %lf, roll = %lf, pitch = %lf\n", state_quadri.posZ, state_quadri_d.posZ, old_state_quadri.posZ, state_quadri.roll, state_quadri.pitch);
    //printf("deriv = %lf\n", deriv(old_state_quadri.posZ, state_quadri->posZ, 0, sp));
    U.U1 = 1-(((((state_quadri_d->posZ-state_quadri->posZ)*kpz)-
            (deriv(old_state_quadri.posZ, state_quadri->posZ, 0, sp)*kdz))*m)/
            (cos(state_quadri->roll)*cos(state_quadri->pitch)));

    U.U2 = ((state_quadri_d->roll-state_quadri->roll)*kpp)-
            (deriv(old_state_quadri.roll, state_quadri->roll, 0, sp)*kdp);

    U.U3 = ((state_quadri_d->pitch-state_quadri->pitch)*kpt)-
            (deriv(old_state_quadri.pitch, state_quadri->dotPitch, 0, sp)*kdt);

    U.U4 = ((state_quadri_d->yaw-state_quadri->yaw)*kpps)-
            (deriv(old_state_quadri.yaw, state_quadri->yaw, 0, sp)*kdps);

    old_state_quadri = *state_quadri;

    //printf("U1 = %lf\nU2 = %lf\nU3 = %lf\nU4 = %lf\n", U.U1, U.U2, U.U3, U.U4);

    return U;
}

U_struc U2bin(U_struc U)
{
    float mult_matrix = 0;
#ifdef DEBUG_WARNINGS
#warning les paramètres b et d ne correspondent pas forcément à ceux de mon système
#endif
    float invert_mapmat[4][4] = {{7999,  0   ,  -15970,  333333},
                                {7999, -15970,   0   , -333333},
                                {7999,  0   ,   15970,  333333},
                                {7999,  15970,   0   , -333333}};

    // produit matriciel
    int i = 0;
    float Omd[4] = {0};
    U_struc bin = {0};

#ifdef DEBUG_WARNINGS
#warning vérifier produit matriciel invert_mapmat * motors
#endif

    for (i = 0; i<4; i++)
    {
        mult_matrix =   invert_mapmat[i][0]*U.U1+
                        invert_mapmat[i][1]*U.U2+
                        invert_mapmat[i][2]*U.U3+
                        invert_mapmat[i][3]*U.U4;
        if(mult_matrix <= 0)
        {
            Omd[i] = 0;
        }
        else
        {
            Omd[i] = sqrt(mult_matrix);
        }
    }

    bin.U1 = (Omd[0]+shi)/slo;
    bin.U2 = (Omd[1]+shi)/slo;
    bin.U3 = (Omd[2]+shi)/slo;
    bin.U4 = (Omd[3]+shi)/slo;

//    printf("U1 = %lf, U2 = %lf, U3 = %lf, U4 = %lf\n", bin.U1, bin.U2, bin.U3, bin.U4);

    return bin;
}

