//=====[#include guards - begin]===============================================

#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void userInterfaceInit();
void userInterfaceHomeInit();
void userInterfaceHRandSp02Init();
void userInterfaceUpdateHRandSp02(int32_t heart_rate, int32_t sp02);

bool menuButtonFlagRead();
void menuButtonFlagWrite(bool val);
bool hrAndSp02ButtonFlagRead();
void hrAndSp02ButtonFlagWrite(bool val);
//=====[#include guards - end]=================================================

#endif // _USER_INTERFACE_H_
