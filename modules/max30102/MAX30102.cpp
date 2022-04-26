/** \file max30102.cpp ******************************************************
*
* Project: MAXREFDES117#
* Filename: max30102.cpp
* Description: This module is an embedded controller driver for the MAX30102
*
*
* --------------------------------------------------------------------
*
* This code follows the following naming conventions:
*
* char              ch_pmod_value
* char (array)      s_pmod_s_string[16]
* float             f_pmod_value
* int32_t           n_pmod_value
* int32_t (array)   an_pmod_value[16]
* int16_t           w_pmod_value
* int16_t (array)   aw_pmod_value[16]
* uint16_t          uw_pmod_value
* uint16_t (array)  auw_pmod_value[16]
* uint8_t           uch_pmod_value
* uint8_t (array)   auch_pmod_buffer[16]
* uint32_t          un_pmod_value
* int32_t *         pn_pmod_value
*
* ------------------------------------------------------------------------- */
/*******************************************************************************
* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/
#include "mbed.h"
#include "MAX30102.h"
#include "algorithm.h"
#include "pc_serial_com.h"
#include "user_interface.h"

// *** pin INT max30102 ***
#define PIN_INT  PE_15
#define I2C2_SDA PB_11
#define I2C2_SCL PB_10

I2C i2c(I2C2_SDA, I2C2_SCL);
DigitalIn pinInt(PIN_INT);
PwmOut led(LED3);


//Serial pc(USBTX, USBRX);


#define MAX_BRIGHTNESS 255

uint32_t aun_ir_buffer[500]; //IR LED sensor data
int32_t n_ir_buffer_length;    //data length
uint32_t aun_red_buffer[500];    //Red LED sensor data
int32_t n_sp02; //SPO2 value
int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;   //heart rate value
int8_t  ch_hr_valid;    //indicator to show if the heart rate calculation is valid
uint8_t uch_dummy;



uint32_t un_min, un_max, un_prev_data;  //variables to calculate the on-board LED brightness that reflects the heartbeats
int i;
int32_t n_brightness;
float f_temp;


void calibration_operation_max30102(){


	maxim_max30102_reset(); //resets the MAX30102

	// initialize serial communication at 115200 bits per second:
	//pc.baud(115200);
	//pc.format(8,SerialBase::None,1);
	//wait(1);


	//read and clear status register
	maxim_max30102_read_reg(0,&uch_dummy);

	//wait until the user presses a key
	//while(uartUsb.readable()==0)
	//{
		//pc.printf("\x1B[2J");  //clear terminal program screen
	//	uartUsb.printf("Press any key to start conversion\n\r");
	//	wait(1);
	//}

	//uch_dummy=getchar();

	maxim_max30102_init();  //initializes the MAX30102


	n_brightness=0;
	un_min=0x3FFFF;
	un_max=0;

	n_ir_buffer_length=500; //buffer length of 100 stores 5 seconds of samples running at 100sps

	//read the first 500 samples, and determine the signal range
	for(i=0;i<n_ir_buffer_length;i++)
	{
        while(pinInt.read()==1);   //wait until the interrupt pin asserts

		maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));  //read from MAX30102 FIFO

		if(un_min>aun_red_buffer[i])
			un_min=aun_red_buffer[i];    //update signal min
		if(un_max<aun_red_buffer[i])
			un_max=aun_red_buffer[i];    //update signal max
		//uartUsb.printf("red=");
		//uartUsb.printf("%i", aun_red_buffer[i]);
		//uartUsb.printf(", ir=");
		//uartUsb.printf("%i\n\r", aun_ir_buffer[i]);
	}
	un_prev_data=aun_red_buffer[i];


	//calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

}


void loop_operation_max30102(bool display){
    //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
	i=0;
	un_min=0x3FFFF;
	un_max=0;

	//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
	for(i=100;i<500;i++)
	{
		aun_red_buffer[i-100]=aun_red_buffer[i];
		aun_ir_buffer[i-100]=aun_ir_buffer[i];

		//update the signal min and max
		if(un_min>aun_red_buffer[i])
		un_min=aun_red_buffer[i];
		if(un_max<aun_red_buffer[i])
		un_max=aun_red_buffer[i];
	}

	//take 100 sets of samples before calculating the heart rate.
	for(i=400;i<500;i++)
	{
		un_prev_data=aun_red_buffer[i-1];
		while(pinInt.read()==1);
		maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));

		if(aun_red_buffer[i]>un_prev_data)
		{
			f_temp=aun_red_buffer[i]-un_prev_data;
			f_temp/=(un_max-un_min);
			f_temp*=MAX_BRIGHTNESS;
			n_brightness-=(int)f_temp;
			if(n_brightness<0)
				n_brightness=0;
		}
		else
		{
			f_temp=un_prev_data-aun_red_buffer[i];
			f_temp/=(un_max-un_min);
			f_temp*=MAX_BRIGHTNESS;
			n_brightness+=(int)f_temp;
			if(n_brightness>MAX_BRIGHTNESS)
				n_brightness=MAX_BRIGHTNESS;
		}

		led.write(1-(float)n_brightness/256);

		//send samples and calculation result to terminal program through UART
//		uartUsb.printf("red=");
//		uartUsb.printf("%i", aun_red_buffer[i]);
//		uartUsb.printf(", ir=");
//		uartUsb.printf("%i", aun_ir_buffer[i]);
//		uartUsb.printf(", HR=%i, ", n_heart_rate);
//		uartUsb.printf("HRvalid=%i, ", ch_hr_valid);
//		uartUsb.printf("SpO2=%i, ", n_sp02);
//		uartUsb.printf("SPO2Valid=%i\n\r", ch_spo2_valid);
		printHeartRateAndOxygenSaturation(aun_red_buffer[i], aun_ir_buffer[i], n_heart_rate, ch_hr_valid, n_sp02, ch_spo2_valid);
	}

	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

	if(display){
		userInterfaceUpdateHRandSp02(n_heart_rate, n_sp02);
	}
}


bool maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
/**
* \brief        Write a value to a MAX30102 register
* \par          Details
*               This function writes a value to a MAX30102 register
*
* \param[in]    uch_addr    - register address
* \param[in]    uch_data    - register data
*
* \retval       true on success
*/
{
  char ach_i2c_data[2];
  ach_i2c_data[0]=uch_addr;
  ach_i2c_data[1]=uch_data;
  
  if(i2c.write(I2C_WRITE_ADDR, ach_i2c_data, 2, false)==0)
    return true;
  else
    return false;
}

bool maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
/**
* \brief        Read a MAX30102 register
* \par          Details
*               This function reads a MAX30102 register
*
* \param[in]    uch_addr    - register address
* \param[out]   puch_data    - pointer that stores the register data
*
* \retval       true on success
*/
{
  char ch_i2c_data;
  ch_i2c_data=uch_addr;
  if(i2c.write(I2C_WRITE_ADDR, &ch_i2c_data, 1, true)!=0)
    return false;
  if(i2c.read(I2C_READ_ADDR, &ch_i2c_data, 1, false)==0)
  {
    *puch_data=(uint8_t) ch_i2c_data;
    return true;
  }
  else
    return false;
}

bool maxim_max30102_init()
/**
* \brief        Initialize the MAX30102
* \par          Details
*               This function initializes the MAX30102
*
* \param        None
*
* \retval       true on success
*/
{
  if(!maxim_max30102_write_reg(REG_INTR_ENABLE_1,0xc0)) // INTR setting
    return false;
  if(!maxim_max30102_write_reg(REG_INTR_ENABLE_2,0x00))
    return false;
  if(!maxim_max30102_write_reg(REG_FIFO_WR_PTR,0x00))  //FIFO_WR_PTR[4:0]
    return false;
  if(!maxim_max30102_write_reg(REG_OVF_COUNTER,0x00))  //OVF_COUNTER[4:0]
    return false;
  if(!maxim_max30102_write_reg(REG_FIFO_RD_PTR,0x00))  //FIFO_RD_PTR[4:0]
    return false;
  if(!maxim_max30102_write_reg(REG_FIFO_CONFIG,0x0f))  //sample avg = 1, fifo rollover=false, fifo almost full = 17
    return false;
  if(!maxim_max30102_write_reg(REG_MODE_CONFIG,0x03))   //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
    return false;
  if(!maxim_max30102_write_reg(REG_SPO2_CONFIG,0x27))  // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
    return false;
  
  if(!maxim_max30102_write_reg(REG_LED1_PA,0x24))   //Choose value for ~ 7mA for LED1
    return false;
  if(!maxim_max30102_write_reg(REG_LED2_PA,0x24))   // Choose value for ~ 7mA for LED2
    return false;
  if(!maxim_max30102_write_reg(REG_PILOT_PA,0x7f))   // Choose value for ~ 25mA for Pilot LED
    return false;
  return true;  
}

bool maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
/**
* \brief        Read a set of samples from the MAX30102 FIFO register
* \par          Details
*               This function reads a set of samples from the MAX30102 FIFO register
*
* \param[out]   *pun_red_led   - pointer that stores the red LED reading data
* \param[out]   *pun_ir_led    - pointer that stores the IR LED reading data
*
* \retval       true on success
*/
{
  uint32_t un_temp;
  unsigned char uch_temp;
  *pun_red_led=0;
  *pun_ir_led=0;
  char ach_i2c_data[6];
  
  //read and clear status register
  maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);
  maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);
  
  ach_i2c_data[0]=REG_FIFO_DATA;
  if(i2c.write(I2C_WRITE_ADDR, ach_i2c_data, 1, true)!=0)
    return false;
  if(i2c.read(I2C_READ_ADDR, ach_i2c_data, 6, false)!=0)
  {
    return false;
  }
  un_temp=(unsigned char) ach_i2c_data[0];
  un_temp<<=16;
  *pun_red_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[1];
  un_temp<<=8;
  *pun_red_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[2];
  *pun_red_led+=un_temp;
  
  un_temp=(unsigned char) ach_i2c_data[3];
  un_temp<<=16;
  *pun_ir_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[4];
  un_temp<<=8;
  *pun_ir_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[5];
  *pun_ir_led+=un_temp;
  *pun_red_led&=0x03FFFF;  //Mask MSB [23:18]
  *pun_ir_led&=0x03FFFF;  //Mask MSB [23:18]
  
  
  return true;
}

bool maxim_max30102_reset()
/**
* \brief        Reset the MAX30102
* \par          Details
*               This function resets the MAX30102
*
* \param        None
*
* \retval       true on success
*/
{
    if(!maxim_max30102_write_reg(REG_MODE_CONFIG,0x40))
        return false;
    else
        return true;    
}
