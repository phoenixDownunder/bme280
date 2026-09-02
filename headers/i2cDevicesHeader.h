/* i2cDevicesHeader.h */

/*
BSD-3-Clause

Copyright (c) 2026 Bruce Stephens
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.

IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * A General Header file with various DEFINES and GLOBAL variables
 * used by ALL devices.
 *
 * February 2023.
 * Amended April 2023.
 * Amended May 2023.
 * Amended February 2024.
 * Amended August 2024.
 * Amended February 2025.
 * Amended September 2025. Expected argc max increased from 11 to 16.
 *
 * This header defines which device is to be built.
 * The default configuration is for the TMP117
 * The second configuration is for the oled SSD1306 display.
 * Other differences will be defined in specific device headers.
 * for example i2ctmp117Header.h and i2cSSD1306Header.h
 * Yes, the use of Header and .h is redundant but convenient.
 * Typically using ...Handlers.h.
 * The Buffer sizes are defined globally as for each device they differ.
 *
 * i2c Adapters and Slave Devices...
 * The range of adapter available on a system normally between 0-99.
 * This is a difficult value to specify because
 * differing systems use i2c interfaces for different reasons.
 * For example RPi4 allows use of the /dev/i2c-1 adapter while
 * reserving /dev/i2c-20 and 21 for its own uses.
 * The Apple MacBook...
 * (specifically an ancient 2008 13" model running Linux Mint)
 * uses adapters /dev/i2c-0 through to /dev/i2c-9.
 * For the purposes of the RPi4, limit the search range to say 99.
 * Ignore RPi4 /dev/i2c-20 and 21.
 *
 * See ./main scan and ./main listadapters
 *
 * For __u8... #include <linux/i2c-dev.h> in i2cMaster.h
 */

#ifndef I2C_DEVICES_HEADER_H
#define I2C_DEVICES_HEADER_H

/*
 * DEFINES the current compile device
 * The Devices...
 * #define GENERIC
 * #define TMP117
 * #define SSD1306
 * #define BME280
 * #define ICM20948
 * #define AK09916
 * #define AK09918
 * #define TCA9548A
 * #define PCA9865
 * #define BME688
 */

/* Common... */
#define MAX_ADAPTER_RANGE 99
#define EXPECTEDARGCMAX   16
#define SHAREDMEMORYSIZE  4096
#define BANKSIZE          0x80
#define BANKNUMBER        4

#ifdef GENERIC
#define INPUT_I2C_BUFFERSIZE  64
#define OUTPUT_I2C_BUFFERSIZE 64
#define EXPECTEDADAPTER       1
#define EXPECTEDSLAVEADDRESS  0x48
#define EXPECTEDSLAVEIDREG    0x00
#define EXPECTEDSLAVEID       0x00
#define FLIP                  0
   static __u8  __attribute__((unused)) g_SlaveIDReg  = EXPECTEDSLAVEIDREG;
   static __u16 __attribute__((unused)) g_SlaveID     = EXPECTEDSLAVEID;
#endif

/*
 * For the TMP117, 64 byte buffer sizes are enough.
 */
#ifdef TMP117
#define INPUT_I2C_BUFFERSIZE  64
#define OUTPUT_I2C_BUFFERSIZE 64
#define EXPECTEDADAPTER       1
#define EXPECTEDSLAVEADDRESS  0x48
#define EXPECTEDSLAVEIDREG    0x0F
#define EXPECTEDSLAVEID       0x0117
#define FLIP                  1
   static __u8  __attribute__((unused)) g_SlaveIDReg  = EXPECTEDSLAVEIDREG;
   static __u16 __attribute__((unused)) g_SlaveID     = EXPECTEDSLAVEID;
#endif

/*
 * For the SSD1306, use 128 byte output buffer (Segment limit) 
 */
#ifdef SSD1306
#define INPUT_I2C_BUFFERSIZE  32
#define OUTPUT_I2C_BUFFERSIZE 128
#define EXPECTEDADAPTER       1
#define EXPECTEDSLAVEADDRESS  0x3C
#define FLIP                  0
#endif

/*
 * For the BME280, use 64 byte output buffer (Segment limit) 
 */
#ifdef BME280
#define INPUT_I2C_BUFFERSIZE  256
#define OUTPUT_I2C_BUFFERSIZE 32
#define EXPECTEDADAPTER       1
#define EXPECTEDSLAVEADDRESS  0x77
#define EXPECTEDSLAVEIDREG    0xD0
#define EXPECTEDSLAVEID       0x60
#define FLIP                  0
   static __u8  __attribute__((unused)) g_SlaveIDReg   = EXPECTEDSLAVEIDREG;
   static __u16 __attribute__((unused)) g_SlaveID      = EXPECTEDSLAVEID;
#endif

/*
 * For the BME688, use 64 byte output buffer (Segment limit) 
 */
#ifdef BME688
/* #define MEMORYMAP */
#define INPUT_I2C_BUFFERSIZE  256
#define OUTPUT_I2C_BUFFERSIZE 32
#define EXPECTEDADAPTER       1
#define EXPECTEDSLAVEADDRESS  0x76
#define EXPECTEDSLAVEIDREG    0xD0
#define EXPECTEDSLAVEID       0x61
#define EXPECTEDVARIANTIDREG  0xF0
#define EXPECTEDVARIANTID     0x01
#define FLIP                  0
   static __u8  __attribute__((unused)) g_SlaveIDReg   = EXPECTEDSLAVEIDREG;
   static __u16 __attribute__((unused)) g_SlaveID      = EXPECTEDSLAVEID;
#endif

/*
 * For the ICM20948, use 64 byte output buffer (Segment limit) 
 * See below for the definitions for the ASAHI KASEI AK09916
 *
 * The icm20948 can have 2 (simple) address options 0x68 and 0x69.
 * The problem is that if 2 icm20948 devices exist on the same i2c bus,
 * there can't be 2 AK09916s enabled at the same time.
 * The AK09916's do not have unique addresses on either icm20948 or
 * optionally modified by the icm20948 address link.
 * Therefore, must use the AK09916 on the i2c bus individually.
 *
 * Assume the default device address is 0x68.
 */
#ifdef ICM20948
#define MEMORYMAP
#define INPUT_I2C_BUFFERSIZE     128
#define OUTPUT_I2C_BUFFERSIZE    128
#define EXPECTEDADAPTER          1
#define EXPECTEDSLAVEADDRESS68   0x68
#define EXPECTEDSLAVEADDRESS69   0x69
#define EXPECTEDSLAVEADDRESS     0x69
#define EXPECTEDSLAVEIDREG       0x00
#define EXPECTEDSLAVEID          0xEA
#define FLIP                     0
/* Default config file PATH path and name */
#define configFName "/home/pi/icm20948.d/icm20948.config"

   static __u8  __attribute__((unused)) g_SlaveIDReg = EXPECTEDSLAVEIDREG;
   static __u16 __attribute__((unused)) g_SlaveID    = EXPECTEDSLAVEID;

#ifdef AK09916
#define AK09916EXPECTEDSLAVEADDRESS 0x0C
#define AK09916EXPECTEDCOMPANYID    0x48
#define AK09916EXPECTEDSLAVEID      0x09
   static __u8 __attribute__((unused)) g_AK09916SlaveAddress = AK09916EXPECTEDSLAVEADDRESS;
   static __u8 __attribute__((unused)) g_AK09916SlaveID      = AK09916EXPECTEDSLAVEID;
#endif
#endif

/*
 * The SparkFun i2c - Multiplexer Switch.
 * Doesn't appear to have any form of ID.
 * Just an 8 i2c port bi-directional switch.
 * Single byte transaction so won't need a large BUFFER.
 */
#ifdef TCA9548A
#define INPUT_I2C_BUFFERSIZE     16
#define OUTPUT_I2C_BUFFERSIZE    16
#define EXPECTEDADAPTER          1
#define EXPECTEDSLAVEADDRESS     0x70
#endif

/*
 * The piicodev servo board.
 * Doesn't appear to have any form of ID.
 * Just an i2c 4 port PWM switch.
 * Single byte transactions so won't need a large BUFFER.
 * Note: This is the piicoDev board PCA9685 address and...
 * NOT the PCA9685 SUBADR0 (0x70) specifically.
 * See PCA9685 software documentation.
 */
#ifdef PCA9685
#define INPUT_I2C_BUFFERSIZE     16
#define OUTPUT_I2C_BUFFERSIZE    16
#define EXPECTEDADAPTER          1
#define EXPECTEDSLAVEADDRESS     0x44
#endif

/*
 * Global static variables.
 * Kept as simple as possible.
 * Only basic addressing parameters specified globally.
 */
   static __u8 __attribute__((unused)) g_Adapter      = EXPECTEDADAPTER;
   static __u8 __attribute__((unused)) g_SlaveAddress = EXPECTEDSLAVEADDRESS;

/* End of i2cDevicesHeader.h */

#endif
