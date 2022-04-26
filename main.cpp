/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */


#include "mbed.h"
#include "arm_book_lib.h"
#include "pulse_oximeter_system.h"


int main() {

	pulseOximeterSystemInit();

    while(1)
    {
    	pulseOximeterSystemUpdate();
    }
}


