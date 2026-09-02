/* i2cMaster.h */

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
 * i2cMaster.h is the Master Header for i2c devices attached to a Raspberry Pi
 * March 2023.
 * Seriously amended April 2023.
 * Amended May 2023.
 * Amended July 2023.
 * Amended February 2024.
 * Amended August 2024.
 * Amended September 2024.
 * Amended February 2025.
 * Cleaned up August 2026.
 *
 * Master read and write procedures for all i2c devices.
 * Could use the often provided wrapper routines but...
 * ... this way allows more control over the fundamental IOCTL routines.
 *
 * Following the principles in...
 * JamesDunne/i2c.c https://gist.github.com/JamesDunne
 * https://gist.github.com/JamesDunne/9b7fbedb74c22ccc833059623f47beb7
 * https://github.com/ExploreEmbedded/8051_DevelopmentBoard/ \
 * blob/master/Code/Keil_Sample_Codes/00-libfiles/oled_i2c.h
 *
 * The Generic i2c Device Defines are in i2cDevicesHeader.h
 * These differ for every i2c device
 *
 * Removed various display options.
 * An explicit result and error handler code is used if required.
 * ERROR HANDLING...
 * For a while various DEFINES for SUCCESS, OK, and for FAIL, ERROR, __ERROR
 * have been used and confusion reigned.
 * For the purposes of SUCCESS or FAILURE, these routine will ALL USE NUMERIC
 *  0 for SUCCESS and 
 * -1 for FAILURE.
 * On the other hand...
 * BOOLEAN variables TRUE and FALSE will be 1 and 0 respectively.
 *
 * All the routines here should compile with...
 * gcc -o <codeFile> <sourceFile>.c -I$HOME/Headers/ -li2c
 * gcc -o <codeFile> <sourceFile>.c -I$HOME/Headers/ -li2c -Wall -ansi
 * gcc -o <codeFile> <sourceFile>.c -I$HOME/Headers/ -li2c -Wall -std=gnu99 -pedantic
 * gcc -o <codeFile> <sourceFile>.c -I$HOME/Headers/ -li2c -Wall -std=c11 -pedantic
 * gcc -o <codeFile> <sourceFile>.c -I$HOME/Headers/ -li2c -Wall -std=c17 -pedantic
 * gcc -o <codeFile> <sourceFile>.c -I$HOME/Headers/ -li2c -Wall -std=c2x -pedantic
 *
 * Needs...
 * Defines needed for the i2cDevicesHeader.h
 * #define ICM20948
 * #define AK09916
 * #define PCA9685
 *
 * #define [other device definitions...]
 *
 * #include <stdio.h>
 * #include <sys/ioctl.h>     / * ioctl * /
 * #include <string.h>
 * #include <errno.h>         / * errno * /
 * #include <linux/i2c-dev.h> / * __u8  * /
 * #include <i2c/smbus.h>
 * #include "_i2cDevicesHeader.h"
 *
 */

#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Main procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

/*
 * int i2cWriteCommand(int fp, __u8 slaveAddr, __u8 reg, __u8 * data, int len) {
 * Returns 0 or -1 on error
 * Up to OUTPUT_BUFFER_SIZE data bytes. 
 * Size is checked in wrappers
 * POLICY here... if an invalid LEN, reject. Could shorten the LEN.
 * Reject because something would be wrong cf the read function.
 * For the SSD1306...
 * typically no more than 4 bytes in a command for the SSD1306
 * Up to 128 data bytes written.
 * Use local buffers here for the write. Slight slowdown but clean...
 *
 * Previously... perror("ioctl(I2C_RDWR) in i2cWriteCommand");
 */
int i2cWriteCommand(int fp, __u8 slaveAddr, __u8 reg, __u8 * data, int len);

/*
 * i2cBlockRead is a simpler routine using i2c_smbus_access rather than IOCTL.
 * There were some timing issues with the ICM20948 using i2cReadByteSequence above
 * as most of the register addresses were not contiguous.
 * i2c_smbus_access is a low level routine defined in i2c-dev.h and uses ioctl(theFile, I2C_SMBUS, &args)
 * Can use for most read transactions
 * The procedure is to read the (raw) data to a RAW DATA BUFFER and...
 * set the corresponding CHECK BUFFER to 1.
 * Each entry in CHECK BUFFER indicates new data is available
 *
 int i2cBlockRead(int fp, __u8 slaveAddress, __u8 startAddress, __u8 length, __u8 * data,  __u8 * check);
 */

/*
 * int i2cWordRead(int fp, __u8 reg, int flip)
 * Uses the low level routine i2c_smbus_access defined in i2c-dev.h
 * IMPORTANT: Possibly Deprecated. There is talk of altering the i2c_smbus_xxx routines.
 * i2c_smbus_access uses ioctl(fp, I2C_SMBUS, &args) and returns __s32 value
 * Can't return -1 here as an error as it maybe valid.
 * Should use this for all read transactions
 * We expect this to always work... If it fails, handle the errno and sort it.
 *
 * int i2cWordRead(int fp, __u8 reg, int flip);
 */

/*
 * int i2cWordWrite(int fp, __u8 reg, int flip)
 * Uses the low level routine i2c_smbus_access defined in i2c-dev.h
 * IMPORTANT: Deprecated. There is talk of altering the i2c_smbus_xxx routines.
 * i2c_smbus_access uses ioctl(fp, I2C_SMBUS, &args)
 * If we can't write to the device, something is wrong. Abort.
 * Should use this for all word write transactions
 *
 * int i2cWordWrite(int fp, __u8 reg, __u16 msg, int flip);
 */

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * .c
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

union byte2 { /* Yes, machine dependent but we are using Raspberry Pis. */
   char byte[2];
   __u16 unsigned_integer;
   __s16 signed_integer;
};

__u16 flipBytes(__u16 word) {
   union byte2 b;
   union byte2 t;
   t.unsigned_integer = word;
   b.byte[0] = t.byte[1];
   b.byte[1] = t.byte[0];
   return b.unsigned_integer;
}

int i2cWriteCommand(int fp, __u8 slaveAddr, __u8 reg, __u8 * data, int len) {
   int i;
   __u8 outbuf[OUTPUT_I2C_BUFFERSIZE + 1] = {0};
   struct i2c_msg msgs[1];
   struct i2c_rdwr_ioctl_data msgset[1];

   if (fp <= 0)  return -1; /* Do basic checks first */
   if (len <  1) return -1;
   if (len > OUTPUT_I2C_BUFFERSIZE) return -1;

   outbuf[0] = reg;
   for (i = 0; i < len; i++) outbuf[i+1] = data[i];

   msgs[0].addr  = slaveAddr;
   msgs[0].flags = 0;
   msgs[0].len   = len + 1;
   msgs[0].buf   = outbuf;

   msgset[0].msgs  = msgs;
   msgset[0].nmsgs = 1;

   if (ioctl(fp, I2C_RDWR, &msgset) < 0) {
      fprintf(stderr, "ioctl(I2C_RDWR) in i2cWriteCommand: %s\n", strerror(errno));
      return -1;
   }
   return 0;
}

/* These wrappers are (were) for icm20948 convenience, technically can use i2c_smbus or SPI.
 * Previously, have been experimenting with i2c_smbus_... routines...
 * Simply wrappers now.
 *
 * NOTE: These are no longer referenced anywhere and are __x__ out.
 * The wrappers were previously used with extemnsive testing and...
 * various options that over time became irrevant.
 * The final option, testing for a valid fp has been left as a reference.
 *
__s32 __x__device_read_byte_data(int fp, __u8 reg) {
   if (fp <= 0) return -1;
   return i2c_smbus_read_byte_data(fp, reg);
}

__s32 __x__device_write_byte_data(int fp, __u8 reg, __u8 value) {
   if (fp <= 0) return -1;
   return i2c_smbus_write_byte_data(fp, reg, value);
}
 */

/*
 * i2c_smbus_access is a low level routine defined in i2c-dev.h and uses ioctl(theFile, I2C_SMBUS, &args)
 * Can use for most read transactions
 */
int i2cBlockRead(int fp, __u8 slaveAddress, __u8 startAddress, __u8 length, __u8 * data,  __u8 * check) {
   union i2c_smbus_data buffer;
   int i = 0;

   if (fp <= 0) return -1;

   buffer.block[0] = length <= 32 ? length : 32;
   for (i = 1; i <= buffer.block[0]; i++) buffer.block[i] = 0;
   for (i = 0; i <  buffer.block[0]; i++) check[startAddress + i] = 0;

   if (i2c_smbus_access(fp, I2C_SMBUS_READ, startAddress, I2C_SMBUS_I2C_BLOCK_DATA, &buffer) ) return -1;

   for (i = 1; i <= buffer.block[0]; i++) {
      data [startAddress + i - 1] = buffer.block[i];
      check[startAddress + i - 1] = 1;
   }
   return (buffer.block[0] == length) ? 0 : -1;
}

int i2cWordRead(int fp, __u8 reg, int flip) {
   union i2c_smbus_data buffer;
   errno = 0;

   if (fp <= 0) return -1;

   if (i2c_smbus_access(fp, I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &buffer) < 0) {
      errno = EIO; /* I/O errno (5) */
      fprintf(stderr, "Unable to read device: %s\n", strerror(errno));
      return -1;
      }
   if (flip) buffer.word = flipBytes(buffer.word);
   return (int) buffer.word;
}

int i2cWordWrite(int fp, __u8 reg, __u16 msg, int flip) {
   union i2c_smbus_data buffer;
   errno = 0;

   if (fp <= 0) return -1;

   buffer.word = msg;
   if (flip) buffer.word = flipBytes(buffer.word);

   if (i2c_smbus_access(fp, I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &buffer) < 0) {
      errno = EIO; /* I/O errno (5) */
      fprintf(stderr, "Unable to write to the device: %s\n", strerror(errno));
      return -1;
      }
   return 0;
}

#endif
