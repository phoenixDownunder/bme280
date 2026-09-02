/* bme280Main.c */

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

/* BME280 
 * Modified April 2025.
 * Update September 2026.
 *
 * BME280 is a combined Temperature, Humidity and Pressure sensor.
 * The first reference is Document Number BST-BME280-DS002-15
 * Revision_1.6_092018
 *
 * The most recent document is BST-BME280-DS001-24 1.24 February 2024.
 *
 * The interface is i2c.
 *
 * Fairly straightforward main routine for talking to the bme280 and copying
 * the humidity, pressure and temperature data to shared memory for any reader to handle.
 *
 * Example commands...
 * bme280 ?     <-- help, then terminate.
 * bme280       <-- with no options, runs once, then termoinates.
 * bme280 -v    <-- displays all bme280 registers and shared memory bank 0.
 * bme280 -n 23 <-- loop 23 times then stop.
 * bme280 -n 0  <-- runs forever.
 * Use ^C to terminate cleanly. ^C sets the global g_quit.
 * Use stop1 to terminate. Sets g_quit using a separate process.
 *
 * Compile using make all or gcc -o bme280 bme280.c -I$HOME/headers/ -li2c -lm
 */

#define BME280

#if __has_include("i2cIncludes.h")
#include "i2cIncludes.h"
#endif

#include "bme280DataStructures.h"
#include "bme280sharedMemoryMaster.h"
#include "bme280Handlers.h"

int main(int argc, char **argv, char *envp[]) {
   int normal_mode = 0;
   int __attribute__((unused)) ignore = 0;
   int i = 0;

   fp = bme280Init(argc, argv); /* fp is a global. */
   if (fp <= 0) exit(0);

   for (i = 0; i < argc; i++) {
        if (strcmp(argv[i],"normal") == 0) normal_mode = 1; /* Ignore case. */
        if (strcmp(argv[i],"NORMAL") == 0) normal_mode = 1;
   }
   if (runLoopInUse) {
      if (normal_mode) ignore = bme280RunLoop(fp, NORMAL_MODE, debug); /* Ignore the 0 return. */
      else             ignore = bme280RunLoop(fp, FORCED_MODE, debug);
   }

   close(fp);
   exit(0);
}
