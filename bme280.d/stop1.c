/* stop1.c */

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
 * C program for process Stop process POSIX shared-memory API.
 * make or gcc -o stop1 stop1.c -I$HOME/headers/ -li2c -lm -Wall -pedantic -std=c2x
 */

#define BME280

#if __has_include("i2cIncludes.h")
#include "i2cIncludes.h"
#endif

#include "i2cCommonSharedMemoryDataStructure.h"
#include "i2cSharedMemoryMaster.h"

   struct timespec ts;    /* Used in icm20948ConvenientBits.h */

int main(int argc, char ** argv) {
   __u8 buffer = 0x01;       /* default is to write 0x01 (stop command) */
   void * ptr_common_shmMap; /* pointer to shared memory object */

   printf("\n");
   printf("Expected command is: %s [option] <enter>\n", argv[0]);
   printf("Options are: 0, 1, -1, 0x00, 0x01(default), 0x02, 0x03, 0xFF\n");

   if (argc > 2) exit(0);

   if (argc == 2) {
      if (strlen(argv[1]) > 4) {
         printf("Argument too long.\n");
         exit(0);
      }
      if (strcmp(toLower(argv[1]), "0")    == 0)  buffer = 0x00;
      if (strcmp(toLower(argv[1]), "1")    == 0)  buffer = 0x01;
      if (strcmp(toLower(argv[1]), "-1")   == 0)  buffer = 0xFF;
      if (strcmp(toLower(argv[1]), "0x00") == 0)  buffer = 0x00;
      if (strcmp(toLower(argv[1]), "0x01") == 0)  buffer = 0x01;
      if (strcmp(toLower(argv[1]), "0x02") == 0)  buffer = 0x02;
      if (strcmp(toLower(argv[1]), "0x03") == 0)  buffer = 0x03;
      if (strcmp(toLower(argv[1]), "0xff") == 0)  buffer = 0xFF;
   }

   printf("\n");
/* Connect to or CREATe Shared memory... */
   ptr_common_shmMap = connectToSharedMemory(shmCName);
   if (ptr_common_shmMap == NULL) {
      printf("Unable to connect to shared memory, exiting.\n");
      exit(0);  /* Just in case. */
   }
   ptr_common_aMap = &common_aMap;
   if (buffer == 0x01) printf("Writing terminateProcess 0x%02X to shared memory...\n", buffer);
   else                printf("Writing 0x%02X to shared memory...\n", buffer);
   writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.terminateProcess, buffer);
   printf("\n");
   displaySharedMemoryBank(0);
   exit(0);
}
