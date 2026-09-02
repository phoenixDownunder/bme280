/* bme280SharedMemoryMaster.h */

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
 * BME280 Shared Memory Read and Write Procedures.
 *
 * August 2024.
 * Amended February 2025.
 * Amended May 2025.
 * Simplified August 2026.
 *
 * It is assumed that the Shared Memory object has been created.
 *
 * Don't alter any pointers, chaos will result.
 */

#ifndef BME280_SHARED_MEMORY_MASTER_H
#define BME280_SHARED_MEMORY_MASTER_H

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Specific BME280 Shared Memory Read and Write procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

/*
 * These are FAR from critical in terms of process to process information transfer.
 * As such, we attempt to lock a simple lock and update rather than suspending threads.
 * Semaphores are not appropriate as we don't want to stop the flow of data.
 * If there is a "semaphore" timeout, we abandon the data write.
 * These are not really required as the shared memory functions are protected anyway.
 */

int writeBME280DataToSharedMemory(void * _ptr_common_shmMap, void * _ptr_common_aMap,
                                  double _aH, double _aP, double _aT) {
   int status = 0;
   if (_ptr_common_shmMap == NULL) return -1;
   if (_ptr_common_aMap   == NULL) return -1;
 
  __u8 rL = readshmByte(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.readLock);

   if (rL != 0) {
      uDelay(5); /* 5mS should be way more than enough. */
      rL = readshmByte(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.readLock);
   }

   if (rL != 0) { /* Blast out any readLock lockup, shouldn't happen. */
      status = writeshmByte(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.readLock, 0x00);
      return -1;
   }

   status |= writeshmByte(  _ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.writeLock, 0x01);
   status |= writeshmDouble(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.humidity, _aH);
   status |= writeshmDouble(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.pressure, _aP);
   status |= writeshmDouble(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.temperature, _aT);
   status |= writeshmByte(  _ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.writeLock, 0x00);

   return status;
}

int readBME280DataFromSharedMemory(void * _ptr_common_shmMap, void * _ptr_common_aMap) {
   int status = 0;
   if (_ptr_common_shmMap == NULL) return -1;
   if (_ptr_common_aMap   == NULL) return -1;

   __u8 wL = readshmByte(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.writeLock);

   if (wL != 0) {
      uDelay(5);
      wL = readshmByte(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.writeLock);
   }

   if (wL != 0) { /* Blast out any writeLock lockup, shouldn't happen. */
      status = writeshmByte(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.writeLock, 0x00);
      return -1;
   }

   status |= writeshmByte(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.readLock, 0x01);
   common_aMap.shm.humidity    = readshmDouble(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.humidity);
   common_aMap.shm.pressure    = readshmDouble(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.pressure);
   common_aMap.shm.temperature = readshmDouble(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.temperature);
   status |= writeshmByte(_ptr_common_shmMap, _ptr_common_aMap, &common_aMap.shm.readLock, 0x00);

   return status;
}

#endif
