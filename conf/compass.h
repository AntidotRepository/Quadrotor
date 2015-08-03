#ifndef COMPASS_H
#define COMPASS_H

#include "hal.h"
#include "GLOBALES.h"

// Task prototype
static WORKING_AREA(waCompass, 256);
msg_t ThreadCompass( void *arg );

// Functions prototypes
void initCompass( void );
float getAngle( void );

#endif //COMPASS_H
