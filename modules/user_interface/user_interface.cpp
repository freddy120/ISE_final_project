//=====[Libraries]=============================================================
#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"
#include "pulse_oximeter_system.h"

#include "display.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_MS 1000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

InterruptIn menuButton(PF_9);
InterruptIn hrAndSp02Button(PG_1);


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============


//=====[Declaration and initialization of private global variables]============

static bool flagMenuButton = false;
static bool flagHrAndSp02Button = false;

//=====[Declarations (prototypes) of private functions]========================

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();

static void menuButtonCallback();
static void hrAndSp02ButtonCallback();

//=====[Implementations of public functions]===================================

void userInterfaceInit()
{
	menuButton.mode(PullUp);
	hrAndSp02Button.mode(PullUp);

	menuButton.fall(&menuButtonCallback);
	hrAndSp02Button.fall(&hrAndSp02ButtonCallback);

    userInterfaceDisplayInit();
}

void userInterfaceHomeInit(){
	displayClear();
	userInterfaceDisplayInit();
}



void userInterfaceHRandSp02Init(){
	displayClear();
	displayCharPositionWrite ( 0,1 );
	displayStringWrite( "HR: " );
	displayCharPositionWrite ( 0,2 );
	displayStringWrite( "SpO2: " );
}

void userInterfaceUpdateHRandSp02(int32_t heart_rate, int32_t sp02){

	displayClear();
	displayCharPositionWrite ( 0,1 );
	displayStringWrite( "HR: " );
	displayCharPositionWrite ( 0,2 );
	displayStringWrite( "SpO2: " );

	char heartRateString[4] = "";
	char sp02String[4] = "";
    sprintf(heartRateString, "%d", heart_rate);
    sprintf(sp02String, "%d", sp02);

	displayCharPositionWrite ( 4,1 );
	displayStringWrite(heartRateString);
	displayCharPositionWrite ( 6,2 );
	displayStringWrite(sp02String);
}

bool menuButtonFlagRead(){
	return flagMenuButton;
}
void menuButtonFlagWrite(bool val){
	flagMenuButton = val;
}
bool hrAndSp02ButtonFlagRead(){
	return flagHrAndSp02Button;
}

void hrAndSp02ButtonFlagWrite(bool val){
	flagHrAndSp02Button = val;
}

//=====[Implementations of private functions]==================================

static void userInterfaceDisplayInit()
{
    displayInit( DISPLAY_TYPE_GLCD_ST7920, DISPLAY_CONNECTION_SPI);

    displayCharPositionWrite ( 0,0 );
    displayStringWrite( "   PULSIMETRO " );
    displayCharPositionWrite ( 0,1 );
    displayStringWrite( "   OXIMETRO " );
    displayCharPositionWrite ( 0,2 );
    displayStringWrite( "Prevencion" );
    displayCharPositionWrite ( 0,3 );
    displayStringWrite( "COVID-19" );
    //displayCharPositionWrite ( 0,1 );
    //displayStringWrite( "Gas:" );

    //displayCharPositionWrite ( 0,2 );
    //displayStringWrite( "Alarm:" );
}


static void menuButtonCallback(){
	flagMenuButton = true;
}
static void hrAndSp02ButtonCallback(){
	flagHrAndSp02Button = true;
}



