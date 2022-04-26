//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_com.h"
#include "date_and_time.h"
#include "light_level_control.h"


//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_COMMANDS,
    PC_SERIAL_GET_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
} pcSerialComMode_t;

//=====[Declaration and initialization of public global objects]===============

Serial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============


//=====[Declaration and initialization of private global variables]============

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
static bool codeComplete = false;
static int numberOfCodeChars = 0;

//=====[Declarations (prototypes) of private functions]========================

static int pcSerialComCommandUpdate( char receivedChar );

static void availableCommands();
static void commandReadPotentiometer();
static void commandSetDateAndTime();
static void commandShowDateAndTime();

//=====[Implementations of public functions]===================================

void printHeartRateAndOxygenSaturation(uint32_t red, uint32_t ir, int32_t hr, int8_t hr_valid, int32_t sp02, int8_t sp02_valid){
	uartUsb.printf("red=");
	uartUsb.printf("%i", red);
	uartUsb.printf(", ir=");
	uartUsb.printf("%i", ir);
	uartUsb.printf(", HR=%i, ", hr);
	uartUsb.printf("HRvalid=%i, ", hr_valid);
	uartUsb.printf("SpO2=%i, ", sp02);
	uartUsb.printf("SPO2Valid=%i\n\r", sp02_valid);
}

void pcSerialComInit()
{
    availableCommands();
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
    	receivedChar = uartUsb.getc();
    }
    return receivedChar;
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.printf( "%s", str );
}

int pcSerialComUpdate()
{
    char receivedChar = pcSerialComCharRead();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_COMMANDS:
                return pcSerialComCommandUpdate( receivedChar );
            default:
                pcSerialComMode = PC_SERIAL_COMMANDS;
                return 0;
        }
    }    
}

bool pcSerialComCodeCompleteRead()
{
    return codeComplete;
}

void pcSerialComCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================


static int pcSerialComCommandUpdate( char receivedChar )
{
    switch (receivedChar) {
        case '1': return 1;
        case 'p': case 'P': commandReadPotentiometer(); return 0;
        case 's': case 'S': commandSetDateAndTime(); return 0;
        case 't': case 'T': commandShowDateAndTime(); return 0;
        default: availableCommands(); return 0;
    }
}

static void availableCommands()
{
    uartUsb.printf( "Available commands:\r\n" );
    uartUsb.printf( "Press '1' Perform HR y SpO2\r\n" );
    uartUsb.printf( "Press 'p' or 'P' to get Potentiometer percentage \r\n" );
    uartUsb.printf( "Press 's' or 'S' to set the date and time\r\n" );
    uartUsb.printf( "Press 't' or 'T' to get the date and time\r\n" );
    uartUsb.printf( "\r\n" );
}


static void commandReadPotentiometer(){
	uartUsb.printf("%POT= %.3f \r\n", lightLevelControlRead()*100.0f);

}
static void commandSetDateAndTime()
{
	int year   = 0;
	int month  = 0;
	int day    = 0;
	int hour   = 0;
	int minute = 0;
	int second = 0;

	uartUsb.printf("\r\nType de current year (YYYY) and press enter: ");
	uartUsb.scanf("%d", &year);
	uartUsb.printf("%d\r\n", year);

	uartUsb.printf("Type de current month (1-12) and press enter: ");
	uartUsb.scanf("%d", &month);
	uartUsb.printf("%d\r\n", month);

	uartUsb.printf("Type de current day (1-31) and press enter: ");
	uartUsb.scanf("%d", &day);
	uartUsb.printf("%d\r\n", day);

	uartUsb.printf("Type de current hour (0-23) and press enter: ");
	uartUsb.scanf("%d", &hour);
	uartUsb.printf("%d\r\n",hour);

	uartUsb.printf("Type de current minutes (0-59) and press enter: ");
	uartUsb.scanf("%d", &minute);
	uartUsb.printf("%d\r\n", minute);

	uartUsb.printf("Type de current seconds (0-59) and press enter: ");
	uartUsb.scanf("%d", &second);
	uartUsb.printf("%d\r\n", second);

	uartUsb.printf("Date and time has been set\r\n");

	while ( uartUsb.readable() ) {
		uartUsb.getc();
	}

	dateAndTimeWrite( year, month, day, hour, minute, second );
}

static void commandShowDateAndTime()
{
    uartUsb.printf("Date and Time = %s", dateAndTimeRead());
}


