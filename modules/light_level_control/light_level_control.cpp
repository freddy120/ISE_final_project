//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "light_level_control.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

AnalogIn potentiometer(A0);
PwmOut ledRed(PA_0);


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void lightLevelControlInit() {
	ledRed.period(0.01f);
}

void lightLevelControlUpdate() {

	ledRed.write(lightLevelControlRead());
}

float lightLevelControlRead()
{
    return potentiometer.read();
}

//=====[Implementations of private functions]==================================
