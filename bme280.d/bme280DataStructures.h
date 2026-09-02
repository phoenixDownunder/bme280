/* bme280DataStructures.h */

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
 * Primary Data Structures for the bme280
 * May 2025.
 *
 * Amended August 2025. 
 * The Data Structures below have been somewhat shortened.
 * Most variables are now local.
 * The results are passed to the COMMON SHARED MEMORY.
 * The results can also be saved to a temporary file.
 */

#ifndef BME280_DATA_STRUCTURES_H
#define BME280_DATA_STRUCTURES_H

#define BME280BUFFERLENGTH 256
#define OFF_MODE    0
#define FORCED_MODE 1
/* There is also FORCED_MODE == 2, but is ignored. Superfluous. */
#define NORMAL_MODE 3

#define g_h_correctionFactor 500 /* Using the bme688 rather than bme280 as reference (3130) */

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * GLOBAL DECLARATIONS for convenience
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

   char * fn = "/tmp/bme280"; /* Treansient results file name */

   struct timespec ts;    /* Used in icm20948ConvenientBits.h */

   static __u8 g_rawDataBuffer[BME280BUFFERLENGTH] = { 0 };
   static __u8 g_dataIsValid[BME280BUFFERLENGTH]   = { 0 };

/* Global device pointer... don't like doing this but convenient. */
   static int fp;

/* The OSRS sampling rates are 0, 1x, 2x, 4x, 8x(the default), 16x */
   __u8  g_osrs_h  = 0x08;
   __u8  g_osrs_p  = 0x08;
   __u8  g_osrs_t  = 0x08;
   __u8  g_filter  = 0x08; /* 000 == filter off, 001=2, 010=4, 011=8, 100+= 16. */
   __u8  g_sb_time = 0x05; /* Binary value used: 000 0.5mS, 001 62.5mS 010 125mS... */
/* The following are reset by the setters, then pulled in at the appropriate time. */
   __u8  s_osrs_h  = 0xFF;
   __u8  s_osrs_p  = 0xFF;
   __u8  s_osrs_t  = 0xFF;
   __u8  s_filter  = 0xFF;
   __u8  s_sb_time = 0xFF;

/* page 29 - 0 sleep mode, 1 & 2 forced mode, 3 normal mode */
/* NORMAL_MODE is only used if you want to turn the bme280
 * into a master device producing environment information stamps.
 * The reading computer and display becomes its slave and suitable for small dedicated systems.
 * The FORCED_MODE is a master-slave request procedure which uses the data as requested.
 * Select "normal" from the command line to run NORMAL mode. The default is FORCED_MODE.
 */

   int g_mode = OFF_MODE; /* Set to Sleep Mode unless sb_time set explicitly. */

   int setterInUse  = 0;
   int runLoopInUse = 0;
   int debug        = 0;
   int _display     = 1; /* Assumes display the results as received. */
   int g_loop       = 1;
   int g_quit       = 0;
   static __s32 g_t_fine  = 0;

/* END of GLOBALS */

#endif
