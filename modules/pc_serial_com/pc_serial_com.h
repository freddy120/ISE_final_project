//=====[#include guards - begin]===============================================

#ifndef _PC_SERIAL_COM_H_
#define _PC_SERIAL_COM_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void pcSerialComInit();
char pcSerialComCharRead();
int pcSerialComUpdate();
void pcSerialComStringWrite( const char* str );
void printHeartRateAndOxygenSaturation(uint32_t red, uint32_t ir, int32_t hr, int8_t hr_valid, int32_t sp02, int8_t sp02_valid);
//=====[#include guards - end]=================================================

#endif // _PC_SERIAL_COM_H_
