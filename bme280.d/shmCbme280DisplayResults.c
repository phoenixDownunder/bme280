/* shmCbme280DisplayResults.c */

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
 * *****************************************************************
 * shmCbme280DisplayResults.c
 * Amended May 2025
 * Amended May 2026
 * Amended September 2026
 *
 * POSIX shared-memory API.
 *
 * make shmCbme280DisplayResults.c
 * gcc -o dest source.c -I$HOME/headers/ -li2c -lm -Wall -pedantic -std=c2x
 * or make all
 *
 * *****************************************************************/

/* Defines the i2c device referenced in i2cDevicesHeader.h */
#define BME280

#if __has_include("i2cIncludes.h")
#include "i2cIncludes.h"
#endif

#include "i2cCommonSharedMemoryDataStructure.h"
#include "i2cSharedMemoryMaster.h"

   struct timespec ts; /* Used in icm20948ConvenientBits.h */

int main() {
   setlocale(LC_ALL, "");
   int optr = 0;

/* shmName is declared in i2cCommonSharedMemeoryDataStructures.h. */
   void * ptr_shm; /* pointer to shared memory object. */
 
   printf("\nAttempt to CONNECT to shared memory %s...\n", shmCName);
   ptr_shm = openSharedMemory(shmCName, SHAREDMEMORYSIZE, 0); /* CONNECT ONLY to Shared memory... */
   if (ptr_shm == NULL)  exit(0);  /* Just in case. */

   ptr_common_shmMap = ptr_shm;
   ptr_common_aMap = &common_aMap;

   printf("\n");
   displaySharedMemoryBank(0);

   printf("Display Temperature, Pressure, Humidity, Gas and Gas Range variables in shared memory bank...\n");
   optr = (int)((long)(&common_aMap.shm.temperature) - (long)(&common_aMap));
   printf("shm --- aT: (Double at Offset: 0x%02X): %.2f %c%cC\n", optr, readshmDouble(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.temperature), 0xC2, 0xB0); 
   optr = (int)((long)(&common_aMap.shm.pressure) - (long)(&common_aMap));
   printf("shm --- aP: (Double at Offset: 0x%02X): %.2f hPa\n", optr, readshmDouble(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.pressure)); 
   optr = (int)((long)(&common_aMap.shm.humidity) - (long)(&common_aMap));
   printf("shm --- aH: (Double at Offset: 0x%02X): %f %%\n", optr, readshmDouble(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.humidity)); 

/* shm_unlink(shmCName); Don't close or remove the shared memory object */
   printf("Exiting, shared memory unchanged.\n");

   exit(0);
}
