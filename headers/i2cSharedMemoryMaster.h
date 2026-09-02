/* i2cCommonSharedMemoryMaster.h */

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
 * COMMON SHARED MEMORY PROCEDURES
 *
 * August 2024.
 * Amended February 2025.
 * Amended May 2025.
 * Amended June 2025.
 *
 * This version is to be independent of the icm20948 device.
 * Designed to be generic with the exceptions of the bme280 procedures.
 *
 * Open or Create a SHAREDMEMORYSIZE shared memory space.
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
 * Don't alter any pointers, chaos will result.
 */

#ifndef I2C_COMMON_SHARED_MEMORY_MASTER_H
#define I2C_COMMON_SHARED_MEMORY_MASTER_H

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Main procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

__u8   readshmByte(void      * _p_shmMap, void * _p_aMap, void * v);
int    writeshmByte(void     * _p_shmMap, void * _p_aMap, void * dest, __u8 v);
double readshmDouble(void    * _p_shmMap, void * _p_aMap, void * v);
int    writeshmDouble(void   * _p_shmMap, void * _p_aMap, void * dest, double v);

int    findOpenSharedMemory(const char * _shmName);
void * openSharedMemory(const char * _shmName, const int shmSize, const int optionCreate);
void * closeSharedMemory(void * _ptr, int shmSize, char * _shmName);
void * connectToSharedMemory(char * _shmName);
void   clearSharedMemory(void * _ptr);

void displayLocalMemoryBank( int bankNumber);
void displaySharedMemoryBank(int bankNumber);

void clearshmLocks();

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Internal procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

void __displayBank(memType_t src, int _bankNumber);
/* void __initialiseSharedMemory(void * restrict ptr, const __u8 value); */
void __initialiseSharedMemory(void * ptr, const __u8 value);
int ftruncate(int f, off_t len); /* Convenient ftruncate Prototype to stop gcc complaining */

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Memory Map dependent procedure.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

/* #ifndef MEMORYMAP */

void clearshmLocks() {
   writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.readLock,  0x00);
   writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.writeLock, 0x00);
   return;
}

/* #endif */

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Shared Memory Procedures - VERY simple read and write data.
 * Altered to accept icm20948 and Common pointers.
 * Note -ansi -C90 compatible, mind you nor is anything else.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

__u8 readshmByte(void * _p_shmMap, void * _p_aMap, void * v) {
  if (_p_shmMap == NULL) return 0;
  return ( * (__u8 *)( (long)(_p_shmMap) + (long)(v) - (long)(_p_aMap)) );
}

int writeshmByte(void * _p_shmMap, void * _p_aMap, void * dest, __u8 v) {
   __u8 * p; /* Done this way to avoid -ansi errors. */
   if (_p_shmMap == NULL) return -1;
   p = (__u8 *)( (long)(_p_shmMap) + (long)(dest) - (long)(_p_aMap));
   *p = v;
   return 0;
}

__u16 readshmU16(void * _p_shmMap, void * _p_aMap, void * v) {
  if (_p_shmMap == NULL) return 0;
  return ( * (__u16 *)( (long)(_p_shmMap) + (long)(v) - (long)(_p_aMap)) );
}

int writeshmU16(void * _p_shmMap, void * _p_aMap, void * dest, __u16 v) {
   __u16 * p;
   if (_p_shmMap == NULL) return -1;
   p = (__u16 *)( (long)(_p_shmMap) + (long)(dest) - (long)(_p_aMap));
   *p = v;
   return 0;
}

__u32 readshmU32(void * _p_shmMap, void * _p_aMap, void * v) {
  if (_p_shmMap == NULL) return 0;
  return ( * (__u32 *)( (long)(_p_shmMap) + (long)(v) - (long)(_p_aMap)) );
}

int writeshmU32(void * _p_shmMap, void * _p_aMap, void * dest, __u32 v) {
   __u32 * p;
   if (_p_shmMap == NULL) return -1;
   p = (__u32 *)( (long)(_p_shmMap) + (long)(dest) - (long)(_p_aMap));
   *p = v;
   return 0;
}

double readshmDouble(void * _p_shmMap, void * _p_aMap, void * v) {
  if (_p_shmMap == NULL) return -1;
  return ( * (double *)( (long)(_p_shmMap) + (long)(v) - (long)(_p_aMap)) );
}

int writeshmDouble(void * _p_shmMap, void * _p_aMap, void * dest, double v) {
   double * p;
   if (_p_shmMap == NULL) return -1;
   p = (double *)( (long)(_p_shmMap) + (long)(dest) - (long)(_p_aMap));
   *p = v;
   return 0;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * .c
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

int findOpenSharedMemory(const char * _shmName) {
   int _shm_fd; /* Local only shared memory file descriptor */
   _shm_fd = shm_open(_shmName, O_RDWR, 0666);
   if (_shm_fd < 0) return -1; /* Not open */
/* Do not shm_unlink(_shm_fd) as this destroys the shared memory object */
   return _shm_fd; /* Shared Memory found */
}

void * __setSharedMemoryPtrs(void * _ptr, const char * _shmName) {
   if (strcmp(_shmName, "myCommonShm") == 0) {
      ptr_common_shmMap = _ptr;
      ptr_common_aMap   = &common_aMap;
   }
/*
   if (strcmp(_shmName, "myShm") == 0) {
      ptr_shmMap = _ptr;
      ptr_aMap   = &aMap;
   }
*/
   return _ptr;
}

void * openSharedMemory(const char * _shmName, const int shmSize, const int optionCreate) {
   void * _ptr;
   int _shm_fd;
/* This routine is a variant of the openSharedMemory routine for the icm20948. */
/* Attempt to find current Open Shared memory... may or may not exist. */

   if (optionCreate) 
      _shm_fd = shm_open(_shmName, O_CREAT | O_RDWR, 0666);
   else
      _shm_fd = shm_open(_shmName, O_RDWR, 0666); /* Yes, kinda redundant. */

   if (_shm_fd < 0) {
      if (optionCreate) {
         errno = ENOMEM;
         fprintf(stderr, "openSharedMemory: Unable to create shared memory: %s\n", strerror(errno));
      } else {
         errno = ENODEV;
         fprintf(stderr, "openSharedMemory: Unable to open shared memory: %s\n", strerror(errno));
      }
      return NULL;
   }
/* Sets the size of the shared memory object. */
   ftruncate(_shm_fd, shmSize);

/* pointer to shared memory object can only exist after the shared Memory Object has been created */
   _ptr = mmap(NULL, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, _shm_fd, 0);
   if (_ptr == MAP_FAILED) {
      shm_unlink(_shmName); 
      errno = ENOMEM;
      fprintf(stderr, "openSharedMemory: mmap open shared memory mapping failed: %s\n", strerror(errno));
      return NULL;
   }
/* _shm_fd is local and is now discarded. Return the (global) ptr to the shared Memory Map. */
   _ptr = __setSharedMemoryPtrs(_ptr, _shmName);
   return _ptr;
}

void * connectToSharedMemory(char * _shmName) {
   void * _ptr;
   int _shm_fd = 0;
/* First determine if the shared memory is already open. */
   _shm_fd = findOpenSharedMemory(_shmName);
/* if (_shm_fd > 0) printf("Shared Memory already exists, connecting...\n"); */

/* option to CREAT allows for both cases to open. */
   _ptr = openSharedMemory(_shmName, SHAREDMEMORYSIZE, (_shm_fd > 0) ? 0 : 1); /* option 1 to create */
   if (_ptr == NULL) {
      printf("Unable to Open Shared Memory.\n");
      exit(0);
   }
/* _shm_fd is local and is now discarded. Return the (global) ptr to the shared Memory Map. */
   _ptr = __setSharedMemoryPtrs(_ptr, _shmName);
   return _ptr;
}

void * closeSharedMemory(void * _ptr, int shmSize, char * _shmName) {
   if (munmap(_ptr, shmSize) != 0)
      fprintf(stderr, "closeSharedMemory: Unmapping failed: %s\n", strerror(errno));

   shm_unlink(_shmName); 
   return NULL;
}

void __initialiseSharedMemory(void * _ptr, const __u8 value) {
   int i = 0;
   union common_shmMap buffer;
   if (_ptr == NULL) return;
   if (value == 0xFF)  /* A covenient trick... creates a counter. */
      for (i = 8; i < SHAREDMEMORYSIZE - 8; i++) buffer.bytes[i] = i % 256;
   else
      for (i = 0; i < SHAREDMEMORYSIZE; i++) buffer.bytes[i] = value;
   memcpy(_ptr, buffer.bytes, SHAREDMEMORYSIZE);
   return; /* Yes, I know, redundant but it has been convenient. */
}

void clearSharedMemory(void * _ptr) {
    __initialiseSharedMemory(_ptr, 0);
}

/* Displays a single 128 byte bank in hex only of either the...
 * shared memory in hex form or the local process global memory.
 * Uses a local buffer with a copy of the shared memory object.
 * Does not return a status value.
 * Nothing is saved or altered.
 * Depending on the position of the data, there may or may not be a validity value.
 * End of device mapped data structure...
 */

void __displayBank(memType_t src, int _bankNumber) {
   int j, v         = 0;
   int _Offset      = 0x00;
   int  __attribute__((unused)) dvOffset     = 0x00;
   int startAddress = 0; /* buffer index */
   int nvm_addr     = 0;
   int AddressStart = 0x00;
   int  __attribute__((unused)) hasValid     = 0;
   union common_shmMap buffer; /* Use local buffer only */

   if ((_bankNumber < 0) || (_bankNumber > (SHAREDMEMORYSIZE / BANKSIZE))) return;

   startAddress = _bankNumber * BANKSIZE;

   hasValid = 1;
   dvOffset = startAddress;

/* Copy the shared memory object or the aMap object to local buffer */
   if (src == display_aMap) {
      if (ptr_common_aMap   != NULL) memcpy(buffer.bytes, ptr_common_aMap,   SHAREDMEMORYSIZE); }
   else {
      if (ptr_common_shmMap != NULL) memcpy(buffer.bytes, ptr_common_shmMap, SHAREDMEMORYSIZE); }

   if (src == display_shm) printf("Shared Memory: ");
      else printf("Local  Memory: ");
   printf("bank %d\n", _bankNumber);

   printf("0x%04X ", startAddress);
   for (j = 0; j < 0x10; j++) printf("  %x", j);
   for (nvm_addr = _Offset; nvm_addr < BANKSIZE; nvm_addr++) { /* BANKSIZE normally 0x80 or 0x40 here */
      if (nvm_addr < AddressStart) {
         if (nvm_addr % 16 == 0) { printf("\n%02X:\t",nvm_addr); }
         printf("   ");
      } else {
         if (nvm_addr % 16 == 0) printf("\n%02X:\t",nvm_addr);
/* The following lines were/are part of the icm20948 experimental setup. Left here for convenience...
 *       v = (hasValid) ? buffer.bytes[dvOffset + nvm_addr] : 1;
 *       if (src == display_shm)  v = 1; * Simple override for shared memory. * /
 *       if (src == display_aMap) v = 1; * Simple override for shared memory. * /
 */
         v = 1;
         if (v) printf("%02X ", buffer.bytes[startAddress + nvm_addr]);
            else printf("-- ");
      }
   }
   printf("\n\n");
}

void displayLocalMemoryBank( int bankNumber) { __displayBank(display_aMap, bankNumber); }
void displaySharedMemoryBank(int bankNumber) { __displayBank(display_shm,  bankNumber); }

#endif
