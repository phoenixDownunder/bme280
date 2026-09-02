/* i2cAdapterHandlers.h */

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
 * i2cAdapterHandlers.h
 *
 * August 2024.
 * Amended February 2025.
 *
 * Various routines to scan the i2c bus for adapters and i2c Slaves.
 * Raspberry Pi orientated because of the internal use of i2c buses 21 and 22.
 *
 * int __i2cOpenAdapter(__u8 adapter) does just that, return the filePointer to the adapter.
 * If there is an error, returns -1.
 *
 * int __i2cSetSlaveDevice(int _fp, __u8 slave_address) detrmines the status of the specified slave address.
 * If there is an error, returns -1.
 *
 * int scanAdapter(int adapter, int extendedRange)...
 * is the simple equivalent of i2cdetect -y adapter without options.
 * Also displays a read register result if a slave is found.
 * The extendedRange allows scanning to start from slaveAddress 0x00
 * rather than the normal 0x08.
 * Checks that the fp is open, returns on fail.
 * scanAdapter is relatively self-contained and only depends on _i2cMaster.h.
 * The scanAdapter version here is specificially looking for the slaves on
 * addresses 0x69, the icm20948 and the magnetometer on 0x0C,
 * if the i2c interface for the magnetometer has been enabled.
 *
 * Closes everything at the end.
 *
 * void i2cListAdapters(int range, int exclude2021)...
 * Scan for i2c adapters.
 * This is a very simple routine relative to i2cdetect -l.
 * A specific exclusion for the RPi4. /dev/i2c-20 and 21 are used internally. Ignore.
 * Does nothing more than attempt to open and then close. 
 *
 * int __i2cScan(int start, int end, int extendedRange, int exclude2021) does the work.
 */

#ifndef I2C_ADAPTER_HANDLERS_H
#define I2C_ADAPTER_HANDLERS_H

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Main procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

/*
 * int i2cPrepareDevice
 * Joins all the startup routines below together.
 * Opens adapter, set the slave device address, reads the default register and
 * returns file pointer fp on success, any failure returns -1.
 * Does not close the fp on fail as this may be part of a scanner. 
 */
int i2cPrepareDevice(__u8 adapter, __u8 slaveAddress);


/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Internal procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

int  __i2cOpenAdapter(__u8 adapter);
int  __i2cSetSlaveDevice(int _fp, __u8 slave_address);
int  __i2cReadSlaveDeviceAddress(int _fp, __u8 address);

int  __i2cScanAdapter(int adapter, int extendedRange);
int  __i2cScanAdapters(int start, int end, int extendedRange, int exclude2021);
void __i2cListAdapters(int range, int exclude2021);


/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * .c
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

/*
 * Create the i2c adapter device name /dev/i2c-nn and open a filePointer to the adapter.
 * Create and return the file pointer fp or -1 on fail 
 */
int __i2cOpenAdapter(__u8 adapter) {
   int _fp = 0;
   char i2c_device_name[14] = {0};
   snprintf(i2c_device_name, 14, "/dev/i2c-%d", (unsigned int)adapter);
   errno = 0; /* cleanup errno */
   _fp = open(i2c_device_name, I2C_RDWR);
   if (_fp <= 0) {
/*    fprintf(stderr, "Unable to open ADAPTER /dev/i2c-%d.\n", adapter); */
      return -1;
   }
   return _fp;
}

/*
 * Assuming the Adapter has been opened, set the specified slave device.
 * ioctl operates locally.
 * On success return the specified slave address or -1 on fail
 * Don't close the fp on fail as this may be part of a scanner. 
 */
int __i2cSetSlaveDevice(int _fp, __u8 slave_address) {
   errno = 0;
   if (ioctl(_fp, I2C_SLAVE, slave_address) < 0) {
      errno = ENXIO; /* ENXIO (6) No such device or address */
      fprintf(stderr, "Unable to set the I2C_SLAVE address 0x%02X.\n", slave_address);
      return -1;
   }
   return (int) slave_address;
}

/*
 * Assuming the above 2 routines have succeeded,
 * assess that the device is attached and responding.
 * Specifically, read the device address.
 * Not reading any ID at this stage.
 * Don't close the fp on fail as this may be part of a scanner. 
 * Return -1 on fail or the register value on success.
 * int __i2cReadSlaveDeviceAddress(int fp, __u8 adapter, __u8 slaveAddress) {
 * __s32 result = 0;
 * if (fp <= 0) return -1;
 * result = i2c_smbus_read_byte(fp); 
 * if (result < 0) return -1;
 * return (int) result;
 */
int __i2cReadSlaveDeviceAddress(int _fp, __u8 address) {
   if (_fp <= 0) return -1;
   return (int)(i2c_smbus_read_byte_data(_fp, address));
}

int i2cPrepareDevice(__u8 adapter, __u8 slaveAddress) {
   int result = 0;
   int _fp = 0;

   _fp = __i2cOpenAdapter(adapter);
   if (_fp < 0) {
      errno = ENXIO;  /* ENXIO (6) No such device or address. */
      return -1;
   }
   result = __i2cSetSlaveDevice(_fp, slaveAddress);
   if (result < 0) {
      close(_fp);
      return -2; /* A wee trick */
   }
   result = __i2cReadSlaveDeviceAddress(_fp, slaveAddress); /* Result ignored. */
   if (result < 0) {
/*    printf("i2cPrepareDevice: Unable to Read Slave Device 0x%02X.\n", slaveAddress); */
      close(_fp);
      return -3;
   }
   return _fp;
}

/*
 * Stand alone procedures to allow the scanning of the i2c adapters
 * in the same way that i2cdetect does.
 */
int __i2cScanAdapter(int adapter, int extendedRange) {
   int startAddress = (extendedRange) ? 0x00 : 0x08;
   int endAddress   = 0x7F;
   int _fp;
   int i;
   int result = 0;
   char * s = "";
   char str[10];

   _fp = __i2cOpenAdapter(adapter);
   if (_fp <= 0) return -1;

   printf("Scanning i2c Adapter /dev/i2c-%d...\n", adapter);
   printf("     ");
   for (i = 0; i < 0x10; i++) printf("  %x", i);
   printf(" Results");
   for (i = 0x00; i <= endAddress; i++) {
      if (i % 16 == 0) {
         printf("%s\n0x%02X: ", s, i); s = "";
      }
      if (i < startAddress) printf("   ");
      else {
         result = __i2cSetSlaveDevice(_fp, i);
         if (result > 0) {
            result = -1; /* Just in case */
            result = __i2cReadSlaveDeviceAddress(_fp, i); /* fail mode (-1) if not there, clears otherwise. */
            if (i == 0x0c) result = i2c_smbus_read_byte_data(_fp, 0x01); /* Magnetometer ID Register */
            if (i == 0x69) result = i2c_smbus_read_byte_data(_fp, 0x00); /* icm20948 ID Register     */
         }
         if (result < 0) printf("-- ");
            else {
               printf("%02X ", i);
               sprintf(str, "%02X ", result);
               s = concat(s, str);
            }
      }
   }
   printf("\n");
   close(_fp);
   return 0;
}

int __i2cScanAdapters(int start, int end, int extendedRange, int exclude2021) {
   int i;
   int __attribute__((unused)) result;
   printf("\n");
   if (exclude2021) {
      printf("Scanning i2c bus adapters /dev/i2c-%d through to /dev/i2c-%d", start, end);
      printf(" excluding /dev/i2c-20 and /dev/i2c-21");
      printf("...\n\n");
   }
   for (i = start; i <= end; i++) {
      if (exclude2021) if ((i == 20) | (i == 21)) continue;
      result = __i2cScanAdapter(i, extendedRange);
   }
   return 0;
}

void __i2cListAdapters(int range, int exclude2021) {
   int fp = 0;
   int adapter       = 0;
   int adapterFinish = range;
   if (exclude2021) printf("Scanning i2c buses (ex 20:21)...\n");
      else printf("Scanning i2c buses...\n");
   for (adapter = 0; adapter < adapterFinish; adapter = ((++adapter == 20) & (exclude2021 == 1)) ? 22 : adapter) {
      fp = __i2cOpenAdapter(adapter);
      if (fp > 0) printf("Adapter /dev/i2c-%d \n", adapter);
      close(fp);
   }
}

#endif
