//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "pulse_oximeter_system.h"
#include "MAX30102.h"
#include "user_interface.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============
int operation_state = 0;
static Ticker ticker;
static bool startHrAndSp02 = false;

//=====[Declarations (prototypes) of private functions]========================

void tickerCallback();

//=====[Implementations of public functions]===================================

void pulseOximeterSystemInit()
{
	calibration_operation_max30102();

    userInterfaceInit();
    pcSerialComInit();
}

void pulseOximeterSystemUpdate(){
	switch(operation_state){
		case 0:

			operation_state = pcSerialComUpdate();

			if(hrAndSp02ButtonFlagRead()){
				operation_state = 1;
				hrAndSp02ButtonFlagWrite(false);
				menuButtonFlagWrite(false);
			}

			if(operation_state == 1){
				startHrAndSp02 = false;
				ticker.attach(&tickerCallback, (float) 5 );

				userInterfaceHRandSp02Init();
			}

			delay(SYSTEM_TIME_INCREMENT_MS);
			break;
		case 1:
			//Perform HR and SPO2

			if(startHrAndSp02){
				ticker.detach();
			}

			loop_operation_max30102(startHrAndSp02);
			if(menuButtonFlagRead()){
				operation_state = 0;
				hrAndSp02ButtonFlagWrite(false);
				menuButtonFlagWrite(false);

				if(!startHrAndSp02){
					ticker.detach();
				}

				userInterfaceHomeInit();
			}

			break;
	}
}

//=====[Implementations of private functions]==================================

void tickerCallback( void )
{
	startHrAndSp02 = true;
}

