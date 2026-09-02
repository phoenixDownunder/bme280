/* i2cIncludes.h */

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
 * i2cIncludes.h
 *
 * August 2024.
 * Amended February 2025.
 *
 * i2cIncludes.h simply lists the necessary COMMON includes for compilation
 * rather than spreading them through the various headers.
 * The order is unfortunately important as some routines are used
 * in differing places.
 *
 * The following is a brutal approach to checking included dependencies.
 * #if __has_include...(...)
 * # include "... .h"
 * #endif
 */

#ifndef I2C_INCLUDES_H
#define I2C_INCLUDES_H

#if defined __has_include

#if __has_include(<locale.h>)
#include <locale.h>
#endif

#if __has_include(<stdio.h>)
#include <stdio.h>
#endif

#if __has_include(<stdlib.h>)
#include <stdlib.h> /* exit and strtol  */
#endif

#if __has_include(<unistd.h>)
#include <unistd.h> /* usleep and close and ftruncate */
#endif

#if __has_include(<fcntl.h>)
#include <fcntl.h> /* open */
#endif

#if __has_include(<sys/ioctl.h>)
#include <sys/ioctl.h> /* ioctl */
#endif

#if __has_include(<string.h>)
#include <string.h>
#endif

#if __has_include(<errno.h>)
#include <errno.h> /* errno */
#endif

#if __has_include(<ctype.h>)
#include <ctype.h> /* for tolower toupper */
#endif

#if __has_include(<time.h>)
#include <sys/time.h> /* gettimeofday */
#include <time.h> /* CLOCKS_PER_SEC and timespec */
#endif

#if __has_include(<math.h>)
#include <math.h>  /* for round */
#endif

#if __has_include(<sys/shm.h>)
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#endif

#if __has_include(<sys/types.h>)
#include <sys/types.h>
#endif

#if __has_include(<linux/i2c-dev.h>)
#include <linux/i2c-dev.h> /* __u8 */
#endif

#if __has_include(<i2c/smbus.h>)
#include <i2c/smbus.h>
#endif

#if __has_include(<lgpio.h>)
#include <lgpio.h>
#endif

#if __has_include(<signal.h>)
#include <signal.h>
#endif

/* Late inclusion stdatomic handlers, really for experimentation.
 * 2011 C11 required. */
#if __has_include(<stdatomic.h>)
#include <stdatomic.h>
#endif

#endif

#include "i2cDevicesHeader.h"
#include "i2cMaster.h"
#include "i2cConvenientBits.h"
#include "i2cAdapterHandlers.h"

#ifdef BME688
#include "signalDataStructures.h"
#endif

#ifdef BME280
#include "i2cCommonSharedMemoryDataStructure.h"
#include "i2cSharedMemoryMaster.h"
#endif

#ifdef BME688
#include "i2cCommonSharedMemoryDataStructure.h"
#include "i2cSharedMemoryMaster.h"
#endif

#ifdef TMP117

#endif

#ifdef TDA9548A

#endif

#endif

/* End of i2cIncludes.h */
