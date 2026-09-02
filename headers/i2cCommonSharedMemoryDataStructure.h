/* i2cCommonSharedMemoryDataStructure.h */

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
 * COMMON SHARED MEMORY DATA STRUCTURE
 *
 * Amended June 2025.
 * Amended August 2025.
 *
 * This version is to be independent of the icm20948 device.
 * Designed to be generic with the exceptions of the bme280 procedures.
 *
 * Open or Create a COMMON SHAREDMEMORYSIZE shared memory space.
 * ftruncate sets the size of the shared memory object.
 * fname is the name of the shared memory object.
 * ptr is the pointer to shared memory object.
 * shm_open is used to open the shared memory object
 * mmap is the memory map the shared memory object
 *
 * A generic 4k map aMap is embedded in these routines.
 *
 * Experimenting here.
 * shm_open without CREAT is not strictly necessary.
 * Could be used to test for a previously open shm object.
 *
 * From the shm_open man description...
 * A new shared memory object initially has zero length.
 * The size of the object can be set using ftruncate(2).
 * The newly allocated bytes of a shared memory object are
 * automatically initialized to 0.
 *
 */

#ifndef I2C_COMMON_SHARED_MEMORY_DATA_STRUCTURE_H
#define I2C_COMMON_SHARED_MEMORY_DATA_STRUCTURE_H

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Defines and Typedefs.
 * If any process has to use this shared memory,..
 * must define its own data structure and DEFINE MEMORYMAP.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

#define SHAREDMEMORYSIZE 4096
/* 
 * shmCName myCommonShm is the name of the common shared memory 
 * for all devices and processes.
 * shmName  myShm is the icm20948 shared memory name.
 */

#define shmCName "myCommonShm"

   typedef enum { /* As declared for the icm20948. */
      display_aMap = 0,
      display_shm
   } memType_t ;

   struct common_memoryMap {
/* 0x0000... */
      __u8 iSlaveAddress68Active;
      __u8 iSlaveAddress69Active;
      __u8 terminateProcess; /* 1 */
      atomic_uchar readLock;
      atomic_uchar writeLock;
      __u8 icm20948InUse;
      __u8 ssd1306InUse;
      __u8 pca9685InUse;
      __u8 reserve0[0x08]; /* 0x10 - 8 */
/* 0x0010... */
      __u8 bme280InUse;
      __u8 bme280_osrs_InUse;
      __u8 bme280_osrs_h;
      __u8 bme280_osrs_p;
      __u8 bme280_osrs_t;
      __u8 bme280_filter;
      __u8 bme280_sb_time;

      __u8 bme688InUse;
      __u8 bme688_osrs_InUse;
      __u8 bme688_osrs_h;
      __u8 bme688_osrs_p;
      __u8 bme688_osrs_t;
      __u8 bme688_filter;
      __u8 bme688_l_time;
      __u8 reserve1[0x02]; /* 0x10 - 14 */
/* 0x0020... */
      int bme688GasMan_active_nb_conv;
      int bme688GasMan_active_gas_wait_time;
      int bme688GasMan_active_gas_wait_time_shared;
      int bme688GasMan_active_res_heat_temp;
/* 0x0030... */
      __u16 bme688GasMan_active_raw_gas;
      __u8  bme688GasMan_active_raw_gas_range;
      __u8  bme688GasMan_active_idac_heat_temp;
      __u8 reserve3[0x0C]; /* 0x10 - 2 - 1 - 1 */
/* 0x0040... */
      double temperature; /* 8 bytes on RPi64 */
      double humidity;
/* 0x0050... */
      double pressure;
      __u8 reserve5[0x08]; /* 0x10 - 8 */
/* 0x0060... */
      double servo1;
      double servo2;
/* 0x0070... */
      double servo3;
      double servo4;
/* 0x0080... */
      __u8 servo1InUse;
      __u8 servo2InUse;
      __u8 servo3InUse;
      __u8 servo4InUse;
      __u8 reserve8[0x0C]; /* 0x10 - 4 */
/* 0x0090... icm20948... */
      double accelX;
      double accelY;
/* 0x00A0... icm20948... */
      double accelZ;
      double gyroX;
/* 0x00B0... icm20948... */
      double gyroY;
      double gyroZ;
/* 0x00C0... AK09916 - icm20948... */
      double magX;
      double magY;
/* 0x00D0... AK09916 - icm20948... */
      double magZ;
      __u8 reserveC[0x08];
/* 0x00E0... */
/* __u8 reserveE[4096 - 0xE0]; The remainder to 4096 */
   };

   static union common_shmMap {
      char bytes[SHAREDMEMORYSIZE];
      struct common_memoryMap shm;
   } common_aMap;

/* Shared Memory pointers - Convenience pointers */
   void * ptr_common_shmMap = common_aMap.bytes; /* Altered later - a safety fudge */
   void * ptr_common_aMap   = common_aMap.bytes; /* Altered later - a safety fudge */

#endif
