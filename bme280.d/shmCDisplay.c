/* shmCDisplay.c */

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
 * shmCDisplay.c
 *
 * Amended  April 2025
 *
 * For testing Shared Memory.
 * See also shmCPrepare.c and shmCRelease.c.
 *
 * C program for Consumer process illustrating POSIX shared-memory API.
 * gcc -o consumer consumer.c -I$HOME/icm20948.d/ -I$HOME/headers/ -li2c -lm -Wall -pedantic -std=c2x
 */

/* Defines needed for the i2cDevicesHeader.h */
#define BME280

#include <time.h>
struct timespec ts; /* Used in icm20948ConvenientBits.h */

#if __has_include("i2cIncludes.h")
#include "i2cIncludes.h"
#endif

int main(int argc, char ** argv) {
int i = 0;
int value = 2; /* Just first 2 banks. */

   printf("%s [n: number of banks to display]\n", argv[0]);

   value = atoi(argv[argc - 1]); /* Use the last argc value. Ignore everything else. */
   if ((value < 1) || (value > 32)) value = 2; /* 32 x 128 == 4096. */

   printf("\nConnecting to %s...\n", shmCName);
   if (openSharedMemory(shmCName, SHAREDMEMORYSIZE, 0) == NULL) exit(0);

   printf("Connection successful.\nWriting 0xDC to readLock...\n\n");
   writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.readLock, 0xDC);
  for (i = 0; i < value; i++) displaySharedMemoryBank(i);
   writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.readLock, 0x00);
   printf("Clearing readLock. Exiting.\n");
// shm_unlink(name); /* remove the shared memory object */
   exit(0);
}
