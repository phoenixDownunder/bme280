/* bme280Handlers.h */

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

/* BME280 Handlers
 * Amended May 2023
 * Modified April 2025.
 * Modified September 2025.
 * Modified October 2025.
 * Modified September 2026.
 *
 * BME280 is a combined Temperature, Humidity and Pressure sensor.
 * The first reference is Document Number BST-BME280-DS002-15
 * Revision_1.6_092018
 *
 * The most recent document is BST-BME280-DS001-24 1.24 February 2024.
 *
 * The interface is i2c.
 *
 * Various structures for convenience below
 *
 * BME280BUFFERLENGTH = 256 characters.
 * This is a convenience fudge.
 * All the useable BME280 registers start at approximately 0x88 up to 0xFE
 * There are some other extraneous registers that are unused.
 * For simplicity, should limit the space used to 0x80 to 0xFF (128 bytes)
 * Unfortunately, the buffer index used typically start at 0x00 and therefore
 * everytime a reference is made to the buffered registers,
 * an OFFSET (0x80) would have to be subtracted everytime to calculate the appropriate index.
 * So rather than starting the buffer from say 0x80 for 128 bytes to 0xFF,
 * use 256 bytes and start everything at 0x00 to 0xFF
 * Two buffers are used for a total of 512 bytes.
 * Previously, bufferLength = sizeof(g_rawDataBuffer) / sizeof(g_rawDataBuffer[0]);
 */

#ifndef BME280_HANDLERS_H
#define BME280_HANDLERS_H

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Main procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

void intHandler(int sig);
void _errorTerminate(int fp, int status);
static __s16 convertToS16(char upper, char lower);
static __u16 convertToU16(char upper, char lower);
void _clearGlobalBuffers();

void bme280PrintHelp();
void displayBME280Registers(int fp);
void bme280Displayosrs_h(int fp);
void bme280Displayosrs_p(int fp);
void bme280Displayosrs_t(int fp);
void bme280DisplayFilterCoefficient(int fp);
void bme280DisplayStandbyTime(int fp);
void bme280DisplayOSRS(int fp);
void bme280DisplayDebugResultSummary(int fp);

int bme280ReadChipID(int fp, int _debug);
int checkDataBlock();
static int waitForReady(int fp, int _debug);

static double _compensateTemperature_ver_1_1(__u32 _adcT, int _debug);
static double _compensatePressure_ver_1_1(__u32 _adcP, int _debug);
static double _compensateHumidity_ver_1_1(__u32 _adcH, int _debug);
static double _compensateHumidityDouble(__u32 _adcH, int _debug);

__u8 _prepareTemperatureSensorOSR(int r);
__u8 _preparePressureSensorOSR(int r);
__u8 _prepareHumiditySensorOSR(int r);
__u8 _prepareSensorMode(int mode);
__u8 _prepareStandbyTime(int t);
__u8 _prepareFilter(int r);

static int _setF2Config(int fp, __u8 v);
static int _setF4Config(int fp, __u8 v);
static int _setF5Config(int fp, __u8 v);

static int setSensorMode(int fp, __u8 _mode, int _debug);
static int setSensorOSRSRegisters(int fp, int _debug);

int _extractTheStaticRegisters(int fp, __u8 slaveAddress);
int _extractTheAltRegisters(int fp, __u8 slaveAddress);
static int extractTheSensorRegisters(int fp, __u8 slaveAddress, int _debug);

static __u32 _recoverTemperatureRegisters(int _debug);
static __u32 _recoverPressureRegisters(int _debug);
static __u32 _recoverHumidityRegisters(int _debug);

static double bme280RecoverTemperature(int fp, int _debug);
static double bme280RecoverPressure(int fp, int _debug);
static double bme280RecoverHumidity(int fp, int _debug);
static double bme280RecoverHumidityDouble(int fp, int _debug);

int writeSensorValuesToFile(char * fn, double t, double p, double h);

void checkArgc(int _argc, char ** _argv);
int checkargvOption(int fp, char * v);
int _doSetter(int argc, char ** argv);
int _doPrepareRunLoop(int argc, char ** argv);
int bme280CommandLineArgumentsPart1(int argc, char ** argv);
int bme280CommandLineArgumentsPart2(int argc, char ** argv);

int bme280PreInit(int argc, char ** argv);
int bme280PostInit(int argc, char ** argv);

int bme280ProcessData(int fp, int _debug);
double calculateDewPoint(int fp, double _temperature, double _relative_humidity);
int bme280CheckConfig(int fp, int _debug);
void bme280Standby(__u8 sb_time);
int bme280RunLoop(int fp, int _mode, int _debug);

int bme280Init(int argc, char ** argv);

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Convenience Handlers.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

void intHandler(int sig) {
   g_quit = 1;
   return;
}

void _errorTerminate(int fp, int status) {
   if (fp < 0) {
      printf("Unable to find device\n");
      exit(0);
    }
   if (status < 0) {
      printf("Invalid status %d\n", status);
      close(fp);
      exit(0);
    }
}

static __s16 convertToS16(char upper, char lower) {
   union byte2 b;
   b.signed_integer = 0; /* clear */
   b.byte[0] = lower;
   b.byte[1] = upper;
   return b.signed_integer;
}

static __u16 convertToU16(char upper, char lower) {
   union byte2 b;
   b.unsigned_integer = 0;
   b.byte[0] = lower;
   b.byte[1] = upper;
   return b.unsigned_integer;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * .c
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

void _clearGlobalBuffers() {
   int index = 0;
   for (index = 0; index < BME280BUFFERLENGTH; index++) {
      g_rawDataBuffer[index] = 0;
      g_dataIsValid[index]   = 0;
   }
   return;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Help details for the bme280. Other parameters are ignored.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

void bme280PrintHelp() {
   char * processName = strrchr(getenv("_"), '/');
   processName++; 
   printf("\n");
   printf("Command format is...\n");
   printf("%s [debug] [nodisplay] [-a adapterAddress][-b slaveAddress] ", processName);
   printf("[-d][-D][f][-h][-m][-p][-s][-t] [0,1,2,4,8(default),16] [-v] [-n nn]\n");
   printf("-h : set over sampling rate 1x 2x 4x 8x 16x for humidity sensor in shared memory, then terminate.\n");
   printf("-p : set over sampling rate 1x 2x 4x 8x 16x for pressure sensor in shared memory, then terminate.\n");
   printf("-t : set over sampling rate 1x 2x 4x 8x 16x for temperature sensor in shared memory, then terminate.\n");
   printf("-f : set the IIR Filter Coefficient 0x 2x 4x 8x 16x in shared memory, then terminate.\n");
   printf("-d : set default osrs for all sensors in shared memory, then terminate.\n");
   printf("-D : set minimum osrs for all sensors in shared memory, then terminate.\n");
   printf("-s : set standby time mS or Seconds for NORMAL mode in shared memory, then terminate.\n");

   printf("   : Use 0.5(mS), 62.5, 125, 250, 500, 1000(mS) or 1(s), 10, 20\n");
   printf("-n : nn set loop mode [0 for infinite or nn seconds] and proceeds to the runLoop\n");
   printf("-m : set mode of operation, Off[0], Forced[1](default), Normal[3] runLoop\n");
   printf("Use %s -n 0 to run continuously (Forced mode with cycle time 1Hz)...\n", processName);

   printf("-v : Display all current device registers, then terminate.\n");
   printf("-? : Display this help summary\n");
   printf("?  : Display this help summary\n");
   printf("Arguments are processed in argument order. All other arguments are ignored\n");
   printf("Use %s debug : run in DEBUG mode.\n", processName);
   printf("Use %s nodisplay : to stop showing the results as they are received. Final results shown.\n", processName);
   printf("Use stop1 or ^C for a controlled interrupt...\n\n");
   return;
}     
      
/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Display the BME280 Registers.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

void displayBME280Registers(int fp) {
   int _Offset      = 0x80;
   int AddressStart = 0x88;
   int AddressEnd   = 0xFF;
   int nvm_addr     = 0;
   if (fp <= 0) {
      printf("BME280 device is not yet open\n");
      return;
   }
   printf("BME280 Registers...");
   for (nvm_addr = _Offset; nvm_addr < (AddressEnd + 1); nvm_addr++) {
      if (nvm_addr < AddressStart) {
         if (nvm_addr % 16 == 0) { printf("\n%02X:\t",nvm_addr); }
         printf("   ");
      } else {
         if (nvm_addr % 16 == 0) { printf("\n%02X:\t",nvm_addr); }
         if (g_dataIsValid[nvm_addr]) {
            printf("%02X ", g_rawDataBuffer[nvm_addr]);
         } else { printf("-- "); }
      }
   }
   printf("\n\n");
   return;
}

void bme280Displayosrs_h(int fp) {
   printf("osr_h oversampling %s (0x%02X)\n",
      (g_osrs_h == 0) ? "off" : (g_osrs_h == 1) ? "x1" : (g_osrs_h == 2)  ? "x2" :
      (g_osrs_h == 4) ? "x4"  : (g_osrs_h == 8) ? "x8" : (g_osrs_h == 16) ? "x16" : "error", g_osrs_h);
   return;
}

void bme280Displayosrs_p(int fp) {
   printf("osr_p oversampling %s (0x%02X)\n",
      (g_osrs_p == 0) ? "off" : (g_osrs_p == 1) ? "x1" : (g_osrs_p == 2)  ? "x2" :
      (g_osrs_p == 4) ? "x4"  : (g_osrs_p == 8) ? "x8" : (g_osrs_p == 16) ? "x16" : "error", g_osrs_p);
   return;
}

void bme280Displayosrs_t(int fp) {
   printf("osr_t oversampling %s (0x%02X)\n",
      (g_osrs_t == 0) ? "off" : (g_osrs_t == 1) ? "x1" : (g_osrs_t == 2)  ? "x2" :
      (g_osrs_t == 4) ? "x4"  : (g_osrs_t == 8) ? "x8" : (g_osrs_t == 16) ? "x16" : "error", g_osrs_t);
   return;
}

void bme280DisplayFilterCoefficient(int fp) {
   printf("Filter Coefficient  %s (0x%02X)\n",
      (g_filter == 0) ? "off" : (g_filter == 1) ? "2" : (g_filter == 2) ? "4" : (g_filter == 3) ? "8" :
      ((g_filter >= 4) & (g_filter < 8)) ? "16" : "error", g_filter);
   return;
}

void bme280DisplayStandbyTime(int fp) {
   printf("Standby Time %s (0x%02X)\n",
      (g_sb_time == 0) ? "0.5mS" : (g_sb_time == 1) ? "62.5mS" : (g_sb_time == 2) ? "125mS" :
      (g_sb_time == 3) ? "250mS" : (g_sb_time == 4) ? "500mS"  : (g_sb_time == 5) ? "1000mS" :
      (g_sb_time == 6) ? "10S"   : (g_sb_time == 7) ? "20S"    : "error", g_sb_time);
   return;
}

void bme280DisplayOSRS(int fp) {
     bme280Displayosrs_h(fp);
     bme280Displayosrs_p(fp);
     bme280Displayosrs_t(fp);
     bme280DisplayFilterCoefficient(fp);
     bme280DisplayStandbyTime(fp);
     return;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Debug summary
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

void bme280DisplayDebugResultSummary(int fp) {
   printf("Shared Memory ptr... PTR_COMMON_SHMMAP... is %p\n", ptr_common_shmMap);
   printf("shm --- aT: %f\n", readshmDouble(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.temperature));
   printf("shm --- aP: %f\n", readshmDouble(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.pressure));
   printf("shm --- aH: %f\n", readshmDouble(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.humidity));
   return;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Should read this register until RESET is complete
 * and the value in this register is the expected value.
 * All i2c_smbus... routines returns a signed 32 bit - in this case the ChipID.
 * The Device register g_SlaveIDReg is 0xD0
 * 250mS delay just in case
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

int bme280ReadChipID(int fp, int _debug) {
   __u8 reg    = 0xD0; /* g_SlaveIDReg */
   __u8 mask   = 0xFF;
   __u8 chipID = 0x00;
   int counter = 0x1F; /* 15 */

   chipID = ( (__u8)i2c_smbus_read_byte_data(fp, reg) ) & mask;
   while ((chipID != g_SlaveID) & (counter > 0)) {
      uDelay(250);
      counter--;
      chipID = ( (__u8)i2c_smbus_read_byte_data(fp, reg) ) & mask;
   }
    if (chipID != g_SlaveID) {
      if (_debug) {
         printf("ChipID at register 0x%02X is 0x%04X. ", g_SlaveIDReg, chipID);
         printf("The expected ChipID 0x%04X NOT found.\n", g_SlaveID);
      }
      return -1;
   }
   if (_debug) {
      printf("Reading ID register 0x%02X, ", reg);
      printf("Chip ID is 0x%02X, expected ID is 0x%02X\n", chipID, g_SlaveID);
   }
   return counter > 0 ? 0 : -1;
}

int checkDataBlock() { /* At reset or start state. */
   if((g_rawDataBuffer[0xF7] == 0x80 ) &
      (g_rawDataBuffer[0xF8] == 0 ) &
      (g_rawDataBuffer[0xF9] == 0 ) &
      (g_rawDataBuffer[0xFA] == 0x80 ) &
      (g_rawDataBuffer[0xFB] == 0 ) &
      (g_rawDataBuffer[0xFC] == 0 ) &
      (g_rawDataBuffer[0xFD] == 0x80 ) &
      (g_rawDataBuffer[0xFE] == 0 )) return -1;
   return 0;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Potentially the code could freeze here if the device throws a hissy fit
 * Really only need to look at the conversion completion status
 * Using a 500mS delay.
 * 0xF3 is the BME280 status register
 * 0x09 = 0b00001001... Bit 3 and bit 0 would be the mask if
 * bit 3 (measuring data) is set to 0 when the most recent conversion
 * has been transferred to the data registers for reading 
 * Bit 1 is set to 1 when the NVM data is being transferred to the image registers
 * before each conversion. 
 * Bit 1 is set to 0 when copying is complete. 
 * Only report on timeout error.
  * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

static int waitForReady(int fp, int _debug) {
   __u8 status  = 0x00;
   int counter  = 0xFF; /* or 256 attempts     */
   __u8 reg     = 0xF3; /* The status register */
   __u8 newDataMask   = 0x01; /* Bit 0 */
   __u8 measuringMask = 0x08; /* Bit 3 */

/* Bit 3 is 1 when conversion is running and 0 when the results have been transferred to the data registers */
/* Bit 0 is 1 when NVM is being copied before every conversion */

   if (fp <= 0) {
      errno = ENXIO; /* ENXIO (6) No such device or address */
      perror("Error: Device is not ready");
      return -1;
   }
   status = i2c_smbus_read_byte_data(fp, reg); /* Clear. */
   if (_debug) { printf("\n"); fflush(stdout); }
   status = i2c_smbus_read_byte_data(fp, reg);
   while (((status & measuringMask) != 0) & (counter > 0)) {
      counter--;
      if (counter % 4 == 0) if (_debug) { printf("."); fflush(stdout); }
      mDelay(1); /* 1mS */
      status = i2c_smbus_read_byte_data(fp, reg);
   }
   if (_debug) printf(" %dmS\n", 256 - counter + 2); /* Plus 2 from the embedded delay in setSensorMode... */
   if (counter <= 1) return -1;
   counter = 0x0F;
   status = i2c_smbus_read_byte_data(fp, reg);
   while (((status & newDataMask) != 0) & (counter > 0)) {
      counter--;
      mDelay(1);
      status = i2c_smbus_read_byte_data(fp, reg);
   }
   return (counter > 0) ? 0 : -1;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Data Compensation Procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

static double _compensateTemperature_ver_1_1(__u32 _adcT, int _debug) {
   double t   = 0.0;
   __s64 var1 = 0;
   __s64 var2 = 0;
   __s64 T1 = (__s64)(convertToU16(g_rawDataBuffer[0x89], g_rawDataBuffer[0x88])); /* __u16 */
   __s64 T2 = (__s64)(convertToS16(g_rawDataBuffer[0x8B], g_rawDataBuffer[0x8A])); /* __s16 */
   __s64 T3 = (__s64)(convertToS16(g_rawDataBuffer[0x8D], g_rawDataBuffer[0x8C])); /* ...   */

   var1 = ( ((_adcT >> 3) - (T1 << 1)) * T2) >> 11;
   var2 = ( ( ( ((_adcT >> 4) - (T1 << 1)) * ((_adcT >> 4) - T1) ) >> 12) * T3) >> 14;
   g_t_fine = var1 + var2; /* Global g_t_fine saved for pressure and humidity calculations. */

   t = (double)((g_t_fine * 5 + 128) >> 8);
   if (_debug) printf("_compensateTemperature: Temperature (* 100) %d\n", (int)(t));
   return t / 100.0;
}

static double _compensatePressure_ver_1_1(__u32 _adcP, int _debug) {
   __s64 p = 1048576 - _adcP; /* the Pressure */
   __s64 var1 = 0;
   __s64 var2 = 0;
   __u64 P1 = (__u64)(convertToU16(g_rawDataBuffer[0x8F], g_rawDataBuffer[0x8E])); /* __u16 */
   __s64 P2 = (__s64)(convertToS16(g_rawDataBuffer[0x91], g_rawDataBuffer[0x90])); /* __s16 */
   __s64 P3 = (__s64)(convertToS16(g_rawDataBuffer[0x93], g_rawDataBuffer[0x92])); /* ...   */
   __s64 P4 = (__s64)(convertToS16(g_rawDataBuffer[0x95], g_rawDataBuffer[0x94]));
   __s64 P5 = (__s64)(convertToS16(g_rawDataBuffer[0x97], g_rawDataBuffer[0x96]));
   __s64 P6 = (__s64)(convertToS16(g_rawDataBuffer[0x99], g_rawDataBuffer[0x98]));
   __s64 P7 = (__s64)(convertToS16(g_rawDataBuffer[0x9B], g_rawDataBuffer[0x9A]));
   __s64 P8 = (__s64)(convertToS16(g_rawDataBuffer[0x9D], g_rawDataBuffer[0x9C]));
   __s64 P9 = (__s64)(convertToS16(g_rawDataBuffer[0x9F], g_rawDataBuffer[0x9E]));

   __s64 shift1_47 = (__s64)(1) << 47;

   var1 = (__s64)(g_t_fine) - 128000; /* Recover global g_t_fine */
   var2 = var1 * var1 * P6;
   var2 = var2 + ((var1 * P5) << 17);
   var2 = var2 + (P4 << 35);
   var1 = ((var1 * var1 * P3) >> 8) + ((var1 * P2) << 12);
/* var1 = (((((__s64)1) << 47) + var1)) * (P1) >> 33; */
   var1 = ((shift1_47 + var1) * P1) >> 33;

   if (var1 == 0) return 0;

   p = ( ((p << 31) - var2) * 3125) / var1;
   var1 = (P9 * (p >> 13) * (p >> 13) ) >> 25;
   var2 = (P8 * p) >> 19;
   p = ((p + var1 + var2) >> 8) + (P7 << 4);

   if (_debug) printf("_compensatePressure: thePressure (before final conversion) 0x%08llX %lld\n", p, p);
/* From the Document, the pressure variable p is in odd Q24.8 bit format. 24 Integer bits and 8 fractional bits. */
   return ((double)(p) / 256.0 ) / 100.0;
}

static double _compensateHumidity_ver_1_1(__u32 _adcH, int _debug) {
   double humidity_max = 102400.0;
   double h = 0.0;
   __u32 adcH = _adcH + g_h_correctionFactor;
   __s64 var1 = g_t_fine - 76800; /* Recover global g_t_fine */
   __s64 H1 = g_rawDataBuffer[0xA1];
   __s64 H2 = convertToS16(g_rawDataBuffer[0xE2], g_rawDataBuffer[0xE1]);
   __s64 H3 = g_rawDataBuffer[0xE3];
   __s64 H4 = ((__s64)(g_rawDataBuffer[0xE4]) << 4) | (g_rawDataBuffer[0xE5] & 0x0F); /* [11:4] [3:0]  */
   __s64 H5 = ((__s64)(g_rawDataBuffer[0xE6]) << 4) | (g_rawDataBuffer[0xE5] >> 4);   /* [3:0]  [11:4] */
   __s64 H6 = g_rawDataBuffer[0xE7];

   var1 = ((((adcH << 14) - (H4 << 20) - (var1 * H5)) + 16384) >> 15) *
           (((((((var1 * H6) >> 10) * (((var1 * H3) >> 11) + 32768) ) >> 10) +
           (__s64)2097152) * H2 + 8192) >> 14);
   var1 = var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * H1) >> 4);
   var1 = (var1 < 0) ? 0 : var1;
   var1 = (var1 > 419430400) ? 419430400 : var1;
   var1 = var1 >> 12;
   h = (double)(var1);
   h = (h < humidity_max) ? h : humidity_max;

   if (_debug) printf("_compensateHumidity: humidity: (h / 1024.0) %.3f%%\n", h / 1024.0);
   return h;
}

static double _compensateHumidityDouble(__u32 _adcH, int _debug) {
   __s64 H1 = g_rawDataBuffer[0xA1];
   __s64 H2 = convertToS16(g_rawDataBuffer[0xE2], g_rawDataBuffer[0xE1]);
   __s64 H3 = g_rawDataBuffer[0xE3];
   __s64 H4 = ((__s64)(g_rawDataBuffer[0xE4]) << 4) | (g_rawDataBuffer[0xE5] & 0x0F); /* [11:4] [3:0]  */
   __s64 H5 = ((__s64)(g_rawDataBuffer[0xE6]) << 4) | (g_rawDataBuffer[0xE5] >> 4);   /* [3:0]  [11:4] */
   __s64 H6 = g_rawDataBuffer[0xE7];

    double humidity;
    double humidity_min = 0.0;
    double humidity_max = 100.0;
    double var1;
    double var2;
    double var3;
    double var4;
    double var5;
    double var6;

    var1 = ((double)g_t_fine) - 76800.0;
    var2 = (((double)H4) * 64.0 + (((double)H5) / 16384.0) * var1);
    var3 = _adcH - var2 + g_h_correctionFactor;
    var4 = ((double)H2) / 65536.0;
    var5 = (1.0 + (((double)H3) / 67108864.0) * var1);
    var6 = 1.0 + (((double)H6) / 67108864.0) * var1 * var5;
    var6 = var3 * var4 * (var5 * var6);
    humidity = var6 * (1.0 - ((double)H1) * var6 / 524288.0);

    humidity = (humidity > humidity_max) ? humidity_max : humidity;
    humidity = (humidity < humidity_min) ? humidity_min : humidity;

   if (_debug) printf("_compensateHumidityDouble: humidity: %f\n", humidity);
    return humidity;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Device Configuration Procedures.
 * Sets the osrs from the global osrs using local variables.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */
    
__u8 _prepareTemperatureSensorOSR(int r) {
   __u8 osr_t = 0x04;
   switch (r) {
      case  0: osr_t = 0x00; break;
      case  1: osr_t = 0x01; break;
      case  2: osr_t = 0x02; break;
      case  4: osr_t = 0x03; break;
      case  8: osr_t = 0x04; break;
      case 16: osr_t = 0x05; break;
      default: osr_t = 0x04; 
   }
   return (osr_t << 5);
}   

__u8 _preparePressureSensorOSR(int r) {
   __u8 osr_p = 0x04;
   switch (r) { /* g_osrs_p */
      case  0: osr_p = 0x00; break;
      case  1: osr_p = 0x01; break;
      case  2: osr_p = 0x02; break;
      case  4: osr_p = 0x03; break;
      case  8: osr_p = 0x04; break;
      case 16: osr_p = 0x05; break;
      default: osr_p = 0x04;
   }
   return (osr_p << 2);
}  
   
__u8 _prepareHumiditySensorOSR(int r) {
   __u8 osr_h = 0x04;
   switch (r) { /* g_osrs_h */
      case  0: osr_h = 0x00; break;
      case  1: osr_h = 0x01; break;
      case  2: osr_h = 0x02; break;
      case  4: osr_h = 0x03; break;
      case  8: osr_h = 0x04; break;
      case 16: osr_h = 0x05; break;
      default: osr_h = 0x04; break;
   }
   return osr_h;
}

__u8 _prepareStandbyTime(int t) {
   if ((t < 0) || (t > 7)) return 0; /* 0.5mS BME280 Datasheet p30. */
/* g_mode = 0x03; */ /* Standby time is only used used Normal mode. */
   return (((__u8)(t)) << 5) & 0xE0; /* 0b 1110 0000 */ 
}

__u8 _prepareFilter(int r) {
   __u8 f = 0x01; /* BME280 Datasheet p30. */
   switch (r) {
      case   0: f = 0x00; break; /* off */
      case   2: f = 0x01; break;
      case   4: f = 0x02; break;
      case   8: f = 0x03; break;
      case  16: f = 0x04; break;
      default: break;
   }
   return (f << 2); /* 0b 0001 1100 Also disables spi... */ 
}

__u8 _prepareSensorMode(int mode) {
/* Normal Mode - 0bttt ppp mm or 0x03 0b0000 0011 */
   __u8 smode = 0x00; /* Sleep mode. */
   switch (mode) {
      case 1:
      case 2: smode = 0x01; break;
      case 3: smode = 0x03; break;
      default: break;
   }
   return smode;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Device Config Registers.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

static int _setF2Config(int fp, __u8 v) {
   __u8 reg   = 0xF2;
   __u8 value = v & 0x07; /* Local copy masked 0b 0000 0111 */
   return i2c_smbus_write_byte_data(fp, reg, value);
}

/* 0xF4 defaults to 0x00 BUT set mode ON or Normal 0x03.
 * osrs_t defaults to 0x00.
 * osrs_p defaults to 0x00.
 * Normal mode is ON or 0x03 or 0 for sleep mode.
 */
static int _setF4Config(int fp, __u8 v) {
   __u8 reg   = 0xF4;
   __u8 value = v; /* local copy */
   return i2c_smbus_write_byte_data(fp, reg, value);
}

/* For F5, the following may exist...
 * tStandby  = 0x01 or 62.5mS
 * Filter coefficient = 0x02
 * enableSPI off
 * tStandby << 5 | tConstant << 1 | enableSPI
 * Set defaults to 0x04.
 * 0b 0000 0100  (000 = 0.5) (001 = filter 2) (0 unused) 0 (Turn off spi)
 */
static int _setF5Config(int fp, __u8 v) {
   __u8 reg      = 0xF5;
   __u8 F5Config = v & 0xFC; /* Turn off spi. */
   return i2c_smbus_write_byte_data(fp, reg, F5Config);
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * This routine reads the global osrs values and sets the sensor
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

static int setSensorMode(int fp, __u8 _mode, int _debug) {
   int i2cResult = 0;
   __u8 reg      = 0xF4;
   __u8 mode     = 0x00; /* Sleep mode is the default. */
   __u8 meas     = 0x00;

   switch ((int)((_mode & 0x03))) {
      case 1 :
      case 2 : mode = 0x01; break;
      case 3 : mode = 0x03; break;
      default: break; /* Sleep mode. */
   }
   g_mode = mode;
   if (_debug && (mode == OFF_MODE)) printf("Off Mode.\n");
   meas = i2c_smbus_read_byte_data(fp, reg) & 0xFC; /* Mask out lower 2 bits. */
   mode = meas | mode;
   i2cResult = i2c_smbus_write_byte_data(fp, reg, mode);
   if (i2cResult < 0) printf("setSensorMODE: Write Error\n");
/* The bme280 is relatively slow. Between the mode write and the response, delay 1mS */
   uDelay(1000);
   return 0;
}

int setSensorOSRSRegisters(int fp, int _debug) {
   int i2cResult  = 0;
   __u8 v = 0x00;

   if (_debug) printf("_setSensorOSRSRegisters: Setting Sensor OSRS Registers...\n");

   setSensorMode(fp, OFF_MODE, 0); /* debug off. */

   v = _prepareHumiditySensorOSR(g_osrs_h);
   if (_debug) printf("_setSensorOSRSRegisters: Write 0x%02X to Register F2\n", v);
   i2cResult = _setF2Config(fp, v);
   if (i2cResult < 0) printf("_setSensorOSRSRegisters: Register F2 Write error.\n");
   _errorTerminate(fp, i2cResult);

   v = (_prepareTemperatureSensorOSR(g_osrs_t) | _preparePressureSensorOSR(g_osrs_p)) & 0xFC;
   v = v | _prepareSensorMode(g_mode);
   if (_debug) printf("_setSensorOSRSRegisters: Write 0x%02X to Register F4\n", v);
   i2cResult = _setF4Config(fp, v);
   if (i2cResult < 0) printf("_setSensorOSRSRegisters: Register F4 Write error.\n");
   _errorTerminate(fp, i2cResult);

   v = _prepareStandbyTime(g_sb_time) | _prepareFilter(g_filter);
   if (_debug) printf("_setSensorOSRSRegisters: Write 0x%02X to Register F5\n", v);
   i2cResult = _setF5Config(fp, v);
   if (i2cResult < 0) printf("_setSensorOSRSRegisters: Register F5 Write error.\n");
   _errorTerminate(fp, i2cResult);

   return 0;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Device Register Recovery Procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

int _extractTheStaticRegisters(int fp, __u8 slaveAddress) {
/* 0x88.. 0xA1 or 26 */
   int i2cResult = i2cBlockRead(fp, slaveAddress, 0x88, 26, g_rawDataBuffer, g_dataIsValid);
/* Exclude E0 Reading the Reset Register */
/* 0xE1... 0xF0 */
   if (i2cResult == 0) i2cResult = i2cBlockRead(fp, slaveAddress, 0xE1, 16, g_rawDataBuffer, g_dataIsValid);
/* See also bme280ReadChipID procedure previously. This just recovers the data in 0xD0. */
/* Read the chip ID */
   if (i2cResult == 0) i2cResult = i2cBlockRead(fp, slaveAddress, 0xD0,  1, g_rawDataBuffer, g_dataIsValid);
   if (i2cResult <  0) {
      printf("Static Register Block Read Error\n");
      close(fp);
      exit(0);
   }
   return 0;
}

int _extractTheAltRegisters(int fp, __u8 slaveAddress) {
/* Read config registers */
   int i2cResult = i2cBlockRead(fp, slaveAddress, 0xF2, 4, g_rawDataBuffer, g_dataIsValid);
   if (i2cResult < 0) {
       printf("Alternate Register Block Read Error\n");
       close(fp);
       exit(0);
   }
   return 0;
}
/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Sensor Raw Data Recovery.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

static int extractTheSensorRegisters(int fp, __u8 slaveAddress, int _debug) {
   int i2cResult = 0;
   __u8 _length  = 0x08;
   __u8 startReg = 0xF7;
   i2cResult = waitForReady(fp, _debug); /* Counter related value returned. Ignore. */
   i2cResult = i2cBlockRead(fp, g_SlaveAddress, startReg, _length, g_rawDataBuffer, g_dataIsValid);
   if (i2cResult <  0) {
      printf("Sensor Register Block Read Error\n");
      close(fp);
      exit(0);
   }
   i2cResult = checkDataBlock(); /* Ignore. Only valid at startup state, not later. */
/* Because this is only relevant at startup, ignore the result. */
   if ((i2cResult < 0) & (_debug)) printf("Data Block CHECK Error: %d\n", i2cResult);
   return 0;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Raw Data Recovery.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

static __u32 _recoverTemperatureRegisters(int _debug) {
   __u8 reg    = 0x00;
   __u32 _adcT = 0x00;
   __u32 msbT  = 0x00;
   __u32 lsbT  = 0x00;
   __u32 xlsbT = 0x00;

   reg = 0xFA;
   msbT = (0x00 | g_rawDataBuffer[reg]) << 12;
   reg = 0xFB;
   lsbT = (0x00 | g_rawDataBuffer[reg]) << 4;
   reg = 0xFC;
   xlsbT = (0x00 | g_rawDataBuffer[reg]) >> 4; /* 0b11110000 >> 4 */

   _adcT = msbT | lsbT | xlsbT;
   if (_debug) {
      printf("_recoverTemperatureRegisters: TEMPERATURE...\n");
      printf("_recoverTemperatureRegisters: Raw Data: msbT 0x%04X lsbT 0x%04X xlsbT 0x%04X\n",
              msbT, lsbT, xlsbT);
      printf("_recoverTemperatureRegisters: uncomp_temperature is: 0x%08X %d\n", _adcT, _adcT);
   }
   return _adcT;
}

static __u32 _recoverPressureRegisters(int _debug) {
   __u8 reg    = 0x00;
   __u32 _adcP = 0x00;
   __u32 msbP  = 0x00;
   __u32 lsbP  = 0x00;
   __u32 xlsbP = 0x00;

   reg = 0xF7;
   msbP = (0x00 | g_rawDataBuffer[reg]) << 12;
   reg = 0xF8;
   lsbP = (0x00 | g_rawDataBuffer[reg]) << 4;
   reg = 0xF9;
   xlsbP = (0x00 | g_rawDataBuffer[reg]) >> 4; /* 0xF0 = 0b11110000 */

   _adcP = msbP | lsbP | xlsbP;
   if (_debug) {
      printf("recoverPressureRegisters: PRESSURE...\n");
      printf("recoverPressureRegisters: Raw  Data: msbP 0x%04X lsbP 0x%04X xlsbP 0x%04X\n",
              msbP, lsbP, xlsbP);
      printf("recoverPressureRegisters: uncomp_pressure is: 0x%08X %d\n", _adcP, _adcP);
   }
   return _adcP;
}

static __u32 _recoverHumidityRegisters(int _debug) {
   __u8 reg    = 0x00;
   __u32 _adcH = 0x00;
   __u32 msbH  = 0x00;
   __u32 lsbH  = 0x00;

   reg = 0xFD;
   msbH = (0x00 | g_rawDataBuffer[reg]) << 8;
   reg = 0xFE;
   lsbH = (0x00 | g_rawDataBuffer[reg]);

   _adcH = msbH | lsbH;
   if (_debug) {
      printf("_recoverHumidityRegisters: HUMIDITY...\n");
      printf("_recoverHumidityRegisters: Raw  Data: msbH 0x%08X, lsbH 0x%08X\n", msbH, lsbH);
      printf("_recoverHumidityRegisters: uncomp_humidity is: 0x%08X %d\n", _adcH, _adcH);
   }
   return _adcH;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Data Recovery Procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

static double bme280RecoverTemperature(int fp, int _debug) {
   double t = 0.0;
   __u32 ut = 0;
   if (fp < 0) return -1.0;
   ut = _recoverTemperatureRegisters(_debug);
   t  = _compensateTemperature_ver_1_1(ut, _debug);
   if (_debug) {
      printf("bme280RecoverTemperature: The raw uncomp_temperature is %u\n", ut);
      printf("bme280RecoverTemperature: Returning Temperature is %7.4f%c%cC\n\n", t, 0xC2, 0xB0);
   }
   return t;
}

static double bme280RecoverPressure(int fp, int _debug) {
   double p = 0.0;
   __u32 up = 0;
   if (fp < 0) return -1.0;
   up = _recoverPressureRegisters(_debug);
   p = _compensatePressure_ver_1_1(up, _debug);
   if (_debug) {
      printf("bme280RecoverPressure: The raw uncomp_pressure is %u\n", up);
      printf("bme280RecoverPressure: Pressure is %f\n", p);
      printf("bme280RecoverPressure: Returning QNH is %.2lfhPa\n\n", p);
   }
   return p;
}

static double bme280RecoverHumidity(int fp, int _debug) {
   double h = 0.0;
   __u32 uh = 0;
   if (fp < 0) return -1.0;
   uh = _recoverHumidityRegisters(_debug);
   h  = _compensateHumidity_ver_1_1(uh, _debug);
   if (_debug) {
      printf("bme280RecoverHumidity: INTEGER raw uncomp_humidity is %u\n", uh);
      printf("bme280RecoverHumidity: INTEGER Humidity is %d (before / 1024)\n", (int)(h));
      printf("bme280RecoverHumidity: INTEGER Returning Humidity is %.2lf%%\n\n", h / 1024.0);
   } 
   return h / 1024.0;
}

static double bme280RecoverHumidityDouble(int fp, int _debug) {
   double h = 0.0;
   __u32 uh = 0;
   if (fp < 0) return -1.0;
   uh = _recoverHumidityRegisters(_debug);
   h  = _compensateHumidityDouble(uh, _debug);
   if (_debug) {
      printf("bme280RecoverHumidity: DOUBLE raw uncomp_humidity is %u\n", uh);
      printf("bme280RecoverHumidity: DOUBLE Returning Humidity is %.2lf%%\n\n", h);
   } 
   return h;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Process data, save to shared memory and file.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

int writeSensorValuesToFile(char * fn, double t, double p, double h) {
/* QFE qfe qnh QNH */
   FILE * fptr;
   char b[8] = {'\0'};
   fptr = fopen(fn, "w");
   if (fptr == NULL) {
      perror("Unable to open the file ");
      return -1;
   }
   snprintf(b, 8, "%5.2f", t);
   fprintf(fptr, "%s\n", b);
   snprintf(b, 8, "%7.2f", p);
   fprintf(fptr, "%s\n", b);
   snprintf(b, 8, "%5.2f", h);
   fprintf(fptr, "%s\n", b);
   fclose(fptr);
   return 0;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Initialise.
 * The initialisation process is split into several parts.
 * The first part is to scan the MAIN arguments for each of the above parts. 
 * The second part used to intialise the device SETTINGS and then terminate.
 * The third part is to recover the run time parameters.
 * The forth part is to initialise the device.
 * The fifth part leads eventually to the device runLoop.
 * The sixth part s the interrupt and terminate process. Separate.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Checks argc and therefore pointers to argv are sensible
 * Simple procedure to eliminate basic rubbish.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

void checkArgc(int _argc, char ** _argv) {
   if ((_argc <= 0) || (_argc > EXPECTEDARGCMAX)) {
      printf("Invalid command line option : argc\n");
      errno = E2BIG;
      perror("Error");
      exit(0);
   }
}

int checkargvOption(int fp, char * v) {
if ((*v != '0') &&
    (*v != '1') &&
    (*v != '2') &&
    (*v != '4') &&
    (*v != '8') &&
    ((v[0] != '1') && (v[1] != '6'))) {
       if (fp > 0) close(fp);
       printf("Error: %s: invalid option\n", v);
       return -1;
   }
   return 0;
}

int _doSetter(int argc, char ** argv) {
/* Replacements for argc and argv
 * Use s = *++argv for convenience... if ( (*++argv)[0] == '-' )...
 */
   int _argc = argc;
   char * s;
   char ** pv;
   char * v;
   pv = argv;
   s = *pv;

   while (--_argc > 0) {
      s = *++pv;
/*    if ( (*++argv or av)[0] == '-' ) ... */
      if ( (s[0] == '-') & (strlen(s) > 1) ) {
/* If this is true then look at the next character in the pv (argv) string */
         s = pv[0] + 1;

         switch (*s) {
/* The following could all be done by character conversion after checking.
 * Easier to test and set the value explicitly.
 */
         case 'h' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv); /* v is a pointer */
                  printf("Humidity Sensor Over Sampling Rate... [0, 1, 2, 4, 8, 16]\n");
                  if (checkargvOption(fp, v) != 0) exit(0);
                  if (strlen(v) == 1) {
                     if (*v == '0') s_osrs_h = 0x00;
                     else
                     if (*v == '1') s_osrs_h = 0x01;
                     else
                     if (*v == '2') s_osrs_h = 0x02;
                     else
                     if (*v == '4') s_osrs_h = 0x04;
                     else
                     if (*v == '8') s_osrs_h = 0x08;
                  }
                  if (strlen(v) == 2) if ( (v[0] == '1') & (v[1] == '6') ) s_osrs_h = 0x10; /* 16. */
                  if (s_osrs_h == 0xFF) printf("Humidity Sensor Sample Rate unchanged\n");
                  else printf("Humidity Sensor Sample Rate s_osrs_h x%d\n", s_osrs_h);
                  setterInUse = 1;      
                  break;
         case 'p' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv); /* v is a pointer */
                  printf("Pressure Sensor Over Sampling Rate... [0, 1, 2, 4, 8, 16]\n");
                  if (checkargvOption(fp, v) != 0) exit(0);
                  if (strlen(v) == 1) {
                     if (*v == '0') s_osrs_p = 0x00;
                     else
                     if (*v == '1') s_osrs_p = 0x01;
                     else
                     if (*v == '2') s_osrs_p = 0x02;
                     else
                     if (*v == '4') s_osrs_p = 0x04;
                     else
                     if (*v == '8') s_osrs_p = 0x08;
                  }
                  if (strlen(v) == 2) if ( (v[0] == '1') & (v[1] == '6') ) s_osrs_p = 0x10; /* 16. */
                  if (s_osrs_p == 0xFF) printf("Pressure Sensor Sample Rate unchanged\n");
                  else printf("Pressure Sensor Sample Rate s_osrs_p x%d\n", s_osrs_p);
                  setterInUse = 1;      
                  break;
         case 't' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv); /* v is a pointer */
                  printf("Temperature Sensor Over Sampling Rate... [0, 1, 2, 4, 8, 16]\n");
                  if (checkargvOption(fp, v) != 0) exit(0);
                  if (strlen(v) == 1) {
                     if (*v == '0') s_osrs_t = 0x00;
                     else
                     if (*v == '1') s_osrs_t = 0x01;
                     else
                     if (*v == '2') s_osrs_t = 0x02;
                     else
                     if (*v == '4') s_osrs_t = 0x04;
                     else
                     if (*v == '8') s_osrs_t = 0x08;
                  }
                  if (strlen(v) == 2) if ( (v[0] == '1') & (v[1] == '6') ) s_osrs_t = 0x10; /* 16. */
                  if (s_osrs_t == 0xFF) printf("Temperature Sensor Sample Rate unchanged\n");
                  else printf("Temperature Sensor Sample Rate s_osrs_t x%d\n", s_osrs_t);
                  setterInUse = 1;      
                  break;
         case 'f' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv); /* v is a pointer */
                  printf("IIR Filter Setting... [0, 2, 4, 8, 16]\n");
                  if (checkargvOption(fp, v) != 0) exit(0);
                  if (strlen(v) == 1) {
                     if (*v == '0') s_filter = 0x00;
                     else
                     if (*v == '2') s_filter = 0x02;
                     else
                     if (*v == '4') s_filter = 0x04;
                     else
                     if (*v == '8') s_filter = 0x08;
                  }
                  if (strlen(v) == 2) if ( (v[0] == '1') & (v[1] == '6') ) s_filter = 0x10; /* 16. */
                  if (s_filter == 0xFF) printf("IIR Filter Coefficient unchanged\n");
                  else printf("IIR Filter Coefficient s_filter set to %d\n", s_filter);
                  setterInUse = 1;      
                  break;
         case 'd' :
                  s_osrs_h  = 0x08;
                  s_osrs_p  = 0x08;
                  s_osrs_t  = 0x08;
                  s_filter  = 0x08;
                  s_sb_time = 0x01; /* 62.5mS BME280 Datasheet p30. */
                  printf("s_osrs_h, s_osrs_p, s_osrs_t Over Sampling x8, s_filter set to 8, s_sb_time set to 1 (62.5mS).\n");
                  setterInUse = 1;      
                  break;
         case 'D' :
                  s_osrs_h  = 0x01;
                  s_osrs_p  = 0x01;
                  s_osrs_t  = 0x01;
                  s_filter  = 0x01;
                  s_sb_time = 0x00; /* 0.5mS BME280 Datasheet p30. */
                  printf("s_osrs_h, s_osrs_p, s_osrs_t Over Sampling x1, s_filter set to 2, s_sb_time set to 0 (0.5mS).\n");
                  setterInUse = 1;      
                  break;
         case 's' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv); /* v is a pointer */
                  s_sb_time = 0xFF;
                  printf("StandbyTime requested: %s...\n", v);
                  if (strcmp(v, "0.5")  == 0) s_sb_time = 0x00; /* 0.5mS BME280 Datasheet p30. */
                  if (strcmp(v, "62.5") == 0) s_sb_time = 0x01; /* 62.5mS */
                  if (strcmp(v, "125")  == 0) s_sb_time = 0x02; /* 125mS */
                  if (strcmp(v, "250")  == 0) s_sb_time = 0x03; /* 250mS */
                  if (strcmp(v, "500")  == 0) s_sb_time = 0x04; /* 500mS */
                  if (strcmp(v, "1")    == 0) s_sb_time = 0x05; /* 1s    */
                  if (strcmp(v, "1000") == 0) s_sb_time = 0x05; /* 1s    */
                  if (strcmp(v, "10")   == 0) s_sb_time = 0x06; /* 10s   */
                  if (strcmp(v, "20")   == 0) s_sb_time = 0x07; /* 20s   */
                  if (s_sb_time == 0xFF) printf("StandbyTime unchanged.\n");
                  else {
                     g_sb_time = s_sb_time;
                     setterInUse = 1;      
                  }
                  bme280DisplayStandbyTime(fp);
                  break;

         default: break;
         }
      }
   }
   return setterInUse;      
}

int _doPrepareRunLoop(int argc, char ** argv) {
/* Replacements for argc and argv
 * Use s = *++argv for convenience... if ( (*++argv)[0] == '-' )...
 */
   char * s;
   int _argc = argc;
   char ** pv;
   char * v;
   pv = argv;
   s = *pv;
   int value = 0;

   while (--_argc > 0) {
      s = *++pv;
/*    if ( (*++argv or av)[0] == '-' ) ... */
      if ( (s[0] == '-') & (strlen(s) > 1) ) {
/* If this is true then look at the next character in the pv (argv) string */
         s = pv[0] + 1;

         switch (*s) {
         case 'a' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv);
                  if (strlen(v) > 2) { printf("Adapter argument %s too long\n", v); exit(0); }
                  value = strtol(v, NULL, 10);
                  if ((value == 20) || (value == 21)) break;
                  if ((value > 0) & (value < 100)) g_Adapter = value;
                  break;
         case 'b' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv);
                  if ( (v[0] == '0') && ((v[1] == 'x') || (v[1] == 'X')) )
                     if ( (strtol(v, NULL, 0) >= 0) && (strtol(v, NULL, 0) < 256) )
                        g_SlaveAddress = (__u8)( (int)(strtol(v, NULL, 0)) ); 
                  break;
         case 'm' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv);
                  g_mode = 0xFF;
                  if (strlen(v) == 1) {
                     if  (*v == '0') g_mode = 0;
                     if ((*v == '1') || (*v == '2')) g_mode = 1;
                     if  (*v == '3') g_mode = 3;
                  }
                  if (g_mode == 0xFF) {g_mode = 0; printf("Invalid mode: %s, Sleep mode enabled.\n", v); }
                  break;
         case 'n' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv);
                  g_loop = 1;
                  if (strlen(v) > 2) { printf("Count argument %s too long\n", v); g_loop = 1; break; }
                  value = strtol(v, NULL, 10);
                  if (value == 0) g_loop = 0;
                  if ((value > 1) & (value < 100)) g_loop = value;
                  break;
          
         default: break;
         }
      }
   }
   return 0;
}

int bme280CommandLineArgumentsPart1(int argc, char ** argv) {
   int doSetter = 0;
   int i = 0;
   for (i = 0; i < argc; i++)  {
      if ((strcmp(argv[i],"-d") == 0) ||
          (strcmp(argv[i],"-D") == 0) ||
          (strcmp(argv[i],"-s") == 0) ||
          (strcmp(argv[i],"-h") == 0) ||
          (strcmp(argv[i],"-p") == 0) ||
          (strcmp(argv[i],"-t") == 0) ||
          (strcmp(argv[i],"-f") == 0)) doSetter = 1;
      if (argv[i][0] == '?') { bme280PrintHelp(); exit(0); }
      if (strcmp(argv[i],"-?") == 0) { bme280PrintHelp(); exit(0); }
   }
   if (debug) printf("DEBUG mode On\n");
   if (doSetter) _doSetter(argc, argv);
   return setterInUse; /* Global. */
}

int bme280CommandLineArgumentsPart2(int argc, char ** argv) {
   int i = 0;
   runLoopInUse = 0;
   for (i = 0; i < argc; i++)  {
      if ((strcmp(argv[i],"-a") == 0) ||
          (strcmp(argv[i],"-b") == 0) ||
          (strcmp(argv[i],"-m") == 0) ||
          (strcmp(argv[i],"-n") == 0)) runLoopInUse = 1;
      if  (strcmp(argv[i],"-v") == 0) {
          runLoopInUse = 0;
          fp = i2cPrepareDevice(g_Adapter, g_SlaveAddress);
          if (fp > 0) {
             _extractTheStaticRegisters(fp, g_SlaveAddress);
             _extractTheAltRegisters(fp, g_SlaveAddress);
             displayBME280Registers(fp);
             bme280DisplayOSRS(fp);
             printf("\n");
             displaySharedMemoryBank(0);
             bme280DisplayDebugResultSummary(fp);
             printf("\n");
             close(fp);
          } else printf("Devce not available.\n"); 
          exit(0);
      }
   }
   if (runLoopInUse) _doPrepareRunLoop(argc, argv);
   runLoopInUse = 1; /* Can occur when no argcs. */
   return 1;
}

int bme280PreInit(int argc, char ** argv) {
   signal(SIGINT, intHandler);
   setlocale(LC_ALL, "");
   __u8 tmp;
   int i;

   _clearGlobalBuffers(); /* The current process whatever that may be. */

/* Connect to or CREATe Shared memory...
 * Need connection to Shared Memory to recover the initial osrs
 * Create defaults in Shared Memory if new.
 * The shared memory if already created may contain the current non zero values.
 * HOWEVER if a setter has been used, one or more g_osrs... entries will have been altered.
 * If the setter has been used, there will be new s_osrs parameters in LOCAL memory.
 * Transfer to Shared Memory.
 * At this stage, the bme280 configuration is unknown.
 * Look at the shared Memory bme280_osrs_InUse to determine if the osrs have been set.
 * If the device is NOT in use, set the GLOBAL oversampling defaults just in case.
 * Humidity    Oversampling RATE osrs_h set to x 8 or 0x04 (as per the bme280 document)
 * Temperature Oversampling RATE osrs_t set to x 8 or 0x04
 * Pressure    Oversampling RATE osrs_p set to x 8 or 0x04
 * Can't write to the device as it isn't ready at this stage.
 */
   ptr_common_shmMap = connectToSharedMemory(shmCName); /* Global pointer. */
   if (ptr_common_shmMap == NULL) {
      printf("Unable to connect to shared memory, exiting.\n");
      exit(0);  /* Just in case. */
   }
   ptr_common_aMap = &common_aMap; /* Global pointer. */

/* Recover the debug options only. */
   for (i = 0; i < argc; i++) if (strcmp(argv[i],"debug")     == 0) debug    = 1;
   for (i = 0; i < argc; i++) if (strcmp(argv[i],"nodisplay") == 0) _display = 0;
/* See later for other argvs... */

   tmp = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_InUse);
   if (tmp == 0x01) {
      if (debug) printf("bme280_osrs_InUse IS set\n");
      g_osrs_h  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_h);
      g_osrs_p  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_p);
      g_osrs_t  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_t);
      g_filter  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_filter);
      g_sb_time = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_sb_time);
   } else {
      printf("bme280_osrs_InUse NOT set\n");
      writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_InUse, 0x01);
      g_osrs_h  = 0x08; /* osr_h = 0x04. */
      g_osrs_p  = 0x08; /* osr_p = 0x04. */
      g_osrs_t  = 0x08; /* osr_t = 0x04. */
      g_filter  = 0x01; /* IIR Filter Coefficient - 001 = 2 */
      g_sb_time = 0x05; /* Standby Time 101 = 1000mS */
      writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_h,  g_osrs_h);
      writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_p,  g_osrs_p);
      writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_t,  g_osrs_t);
      writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_filter,  g_filter);
      writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_sb_time, g_sb_time);
   }

/* Just in case, check for errors. */
   switch(g_osrs_h) {
      case 0: case 1: case 2: case 4: case 8: case 16: break;
      default: g_osrs_h = 0x08; break;
   }
   switch(g_osrs_p) {
      case 0: case 1: case 2: case 4: case 8: case 16: break;
      default: g_osrs_p = 0x08; break;
   }
   switch(g_osrs_t) {
      case 0: case 1: case 2: case 4: case 8: case 16: break;
      default: g_osrs_t = 0x08; break;
   }
   switch(g_filter) {
/*    case 0: case 1: case 3: case 7: case 15: case 31: case 63: case 127: break; bme688. */
      case 0: case 2: case 4: case 8: case 16: break;
      default: g_filter = 0x01; break;
   }
   g_sb_time  = g_sb_time & 0x07; /* constrain. */

   setterInUse = 0; /* Global. */
   setterInUse = bme280CommandLineArgumentsPart1(argc, argv);

   if (setterInUse) {
      writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_InUse, 0x01); /* Permanent. */
      if (s_osrs_h  != 0xFF) writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_h, s_osrs_h);
      if (s_osrs_p  != 0xFF) writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_p, s_osrs_p);
      if (s_osrs_t  != 0xFF) writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_t, s_osrs_t);
      if (s_filter  != 0xFF) writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_filter, s_filter);
      if (s_sb_time != 0xFF) writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_sb_time,s_sb_time);
      if (debug) { /* Doesn't matter about the local process g values. */
         g_osrs_h  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_h);
         g_osrs_p  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_p);
         g_osrs_t  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_t);
         g_filter  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_filter);
         g_sb_time = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_sb_time);
         printf("bme280PreInit: setterInUse:\n");
         bme280DisplayOSRS(fp);
         displaySharedMemoryBank(0);
         printf("setterInUse completed.\n");
      }
     exit(0);
   }
   runLoopInUse = bme280CommandLineArgumentsPart2(argc, argv);
   if (debug) {
      printf("bme280PreInit: \n");
      bme280DisplayOSRS(fp);
      displaySharedMemoryBank(0);
   }
   return runLoopInUse;
}

int bme280PostInit(int argc, char ** argv) {
   int _i2cResult = 0;

/* Shared Memory parameters already sorted... look for a live device. */

   fp = i2cPrepareDevice(g_Adapter, g_SlaveAddress);
   if (fp < 0) {
      printf("Unable to find device 0x%02X on /dev/i2c-%d\n", g_SlaveAddress, g_Adapter);
      exit(0);
   }

   _i2cResult = bme280ReadChipID(fp, debug);
   _errorTerminate(fp, _i2cResult);

   _i2cResult = setSensorOSRSRegisters(fp, debug);
   _errorTerminate(fp, _i2cResult);

   _i2cResult = _extractTheStaticRegisters(fp, g_SlaveAddress);
   _errorTerminate(fp, _i2cResult);
   _i2cResult = _extractTheAltRegisters(fp, g_SlaveAddress);
   _errorTerminate(fp, _i2cResult);

   common_aMap.shm.readLock         = 0;
   common_aMap.shm.writeLock        = 0;
   common_aMap.shm.terminateProcess = 0;
   writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.terminateProcess, 0x00);

   mDelay(10); /* Wait 10mS */

   return fp;
}

int bme280ProcessData(int fp, int _debug) {
   int status = 0;

   status |= extractTheSensorRegisters(fp, g_SlaveAddress, _debug);

   common_aMap.shm.temperature = bme280RecoverTemperature(fp, _debug);
   common_aMap.shm.pressure    = bme280RecoverPressure(fp, _debug); /* 1013.25 Standard QNH */
   common_aMap.shm.humidity    = bme280RecoverHumidityDouble(fp, _debug);
   common_aMap.shm.humidity    = bme280RecoverHumidity(fp, _debug);

   status |= writeBME280DataToSharedMemory(ptr_common_shmMap, ptr_common_aMap,
             common_aMap.shm.humidity, common_aMap.shm.pressure, common_aMap.shm.temperature);

   return status;
}

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
 * Late addition: Calculate Dew Point... June 2026.
 * From DuckDuckGo Search...
 * The Magnus Formula for calculating (in C) the dew point temperature (Td) is:
 *
 * Td = (b * Alpha(T, RH)) / (a - Alpha(T, RH))
 * Where:
 *    Alpha(T, RH) = ln(RH/100) + (a * T) / (b + T)
 *    a = 17.625
 *    b = 243.04
 * It is better to handle the errors beforehand...
 * Error handling removed...
   errno = 0;
   if (errno == EDOM) {
      printf("Domain error: log of %f is undefined (EDOM).\n", _relative_humidity);
      return 0;
   }
   if (errno == ERANGE) {
      printf("Range error: log of %f is invalid (ERANGE).\n", _relative_humidity);
      return 0;
   }
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

double calculateDewPoint(int fp, double _temperature, double _relative_humidity) {
   double a = 17.625;
   double b = 243.04;
   double alpha;
/* Yes, I know I can join all these conditions together... More useable this way. */
   if (_relative_humidity < 0)     return 0; /* LN negative: errno: EDOM. */
   if (_relative_humidity == 0)    return 0; /* LN pole error: errno: ERANGE. */
   if (_relative_humidity < 0.01)  return 0; /* Not sensible: 0.01% _relative humidity. */
   if (_relative_humidity > 100.0) return 100; /* Error: value > 100% relative humidity. */
   if ((_temperature < -40) || (_temperature > 85)) return 0; /* bme280 (p2) temperature operating range. */

   alpha = log(_relative_humidity / 100.0) + ((a * _temperature) / (b + _temperature));
   return (b * alpha) / (a - alpha);
} 

int bme280CheckConfig(int fp, int _debug) {
   __u8 reg = 0xF2;
   __u8 v   = 0x00;
   __u8 s   = 0x00;
   if (_debug) printf("Checking parameters...\n");
   v = i2c_smbus_read_byte_data(fp, reg) & 0x07; /* Humidity Register. */
   s = _prepareHumiditySensorOSR(g_osrs_h) & 0x07;
   if (v != s) printf("Humidity OSRS incorrect: 0x%02X, should be: 0x%02X.\n", s, v);
   reg = 0xF4;
   v = i2c_smbus_read_byte_data(fp, reg) & 0xFC; /* osrs_t and osrs_p Register. Mask out mode. */
   s = (_prepareTemperatureSensorOSR(g_osrs_t) | _preparePressureSensorOSR(g_osrs_p)) & 0xFC;
   if (v != s) printf("Pressure or Temperature OSRS incorrect: 0x%02X, should be: 0x%02X.\n", s, v);
   reg = 0xF5;
   v = i2c_smbus_read_byte_data(fp, reg) & 0x1C; /* Filter Register. */
   s = _prepareFilter(g_filter) & 0x1C;
   if (v != s) printf("Filter OSRS incorrect: 0x%02X, should be: 0x%02X.\n", s, v);

   if (_debug) bme280DisplayOSRS(fp);

   return 0;
}

int bme280Checkshm(int fp, int _debug) {
    int update = 0;
    __u8 p = 0x00;

    p = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_h);
    if (p != g_osrs_h)  { g_osrs_h = p; update = 1; }
    p  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_p);
    if (p != g_osrs_p)  { g_osrs_p = p; update = 1; }
    p  = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_osrs_t);
    if (p != g_osrs_t)  { g_osrs_t = p; update = 1; }
    p = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_filter);
    if (p != g_filter)  { g_filter = p; update = 1; }
    p = readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280_sb_time);
    if (p != g_sb_time) { g_sb_time = p; update = 1; }

    if (update) {
       p = g_mode;
       setSensorMode(fp, OFF_MODE, 0); /*_debug off. */
       setSensorOSRSRegisters(fp, 0);  /* debug off. */
       if (p == NORMAL_MODE) setSensorMode(fp, NORMAL_MODE, 0); /* debug off. */
       if (_display) {
          printf("bme280Checkshm UPDATE...\n");
          bme280DisplayOSRS(fp);
       }
    }
    return 0;
}

void bme280Standby(__u8 sb_time) {
      switch((int)(sb_time)) {
         case 0 : uDelay(500);  break;
         case 1 : mDelay(60);   break;
         case 2 : mDelay(125);  break;
         case 3 : mDelay(250);  break;
         case 4 : mDelay(500);  break;
         case 5 : mDelay(1000); break; /* Wait 1000mS in FORCED_MODE, could use the g_sb_time. */
         case 6 : sleep(10);    break;
         case 7 : sleep(20);    break;
         default: mDelay(1000); break;
      }
}

int bme280RunLoop(int fp, int _mode, int _debug) {
   char * processName = NULL;
   int status = 0;
   int _g_loop = g_loop;

   processName = strrchr(getenv("_"), '/'); processName++;

   if (_debug) {
      printf("\n");
      printf("Use %s -? for command options help.\n", processName);
      printf("Use %s -n 0 to run continuously (cycle time 1Hz)...\n", processName);
      printf("Use stop1 or ^C for a clean interrupt...\n\n");
   }

   bme280CheckConfig(fp, _debug);

   if (_mode == OFF_MODE) {
      printf("g_mode: OFF_MODE.\n");
      setSensorMode(fp, OFF_MODE, 0); /* debug off. */
      printf("g_mode: OFF_MODE.\n");
      return 0;
   }

   if (_mode == NORMAL_MODE) {
      g_mode = _mode;
      if (_debug) printf("\ng_mode: NORMAL_MODE.\n");
      setSensorMode(fp, NORMAL_MODE, 0); /* g_mode is set, debug off. */
   }
   if (_mode == FORCED_MODE) {
      g_mode = _mode;
      if (_debug) printf("\ng_mode: FORCED_MODE.\n");
   }

   g_quit = 0;
   while (!g_quit) {
      if (g_mode == FORCED_MODE) setSensorMode(fp, FORCED_MODE, 0); /* debug off. */
      status = bme280ProcessData(fp, _debug); /* 0 = no debug */
      if (_display || (_g_loop == 1)) {
         printf("Temperature: %.2f%c%c, ", common_aMap.shm.temperature, 0xC2, 0xB0);
         printf("QNH: %.2fhPa, ", common_aMap.shm.pressure);
         printf("Humidity: %.2f%%, ", common_aMap.shm.humidity);
         printf("Dew Point: %.2f%c%c\n", calculateDewPoint(fp, common_aMap.shm.temperature, common_aMap.shm.humidity), 0xC2, 0xB0);
      }
      if (status != 0) { printf("bme280ProcessData: Data processing error.\n"); g_quit = 1; continue; };

      if (_debug) bme280DisplayOSRS(fp);

      if (_g_loop == 1) { g_quit = 1; continue; }
      if (_g_loop  > 1) _g_loop--;
      bme280Checkshm(fp, _debug);
      bme280Standby(g_sb_time);
      g_quit = g_quit || readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.terminateProcess);
   }
   g_quit = 0;
   writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.bme280InUse, 0x00);
/* Don't reset terminateProcess as it may be used to clear associated processes. e.g. display */
   return 0;
}

int bme280Init(int argc, char ** argv) {
   setterInUse  = 0;
   runLoopInUse = 0;
   g_mode = OFF_MODE;
   setterInUse  = bme280PreInit(argc, argv);
   fp = bme280PostInit(argc, argv);
   if (fp <= 0) exit(0);
   if (runLoopInUse != 1) { close(fp); exit(0); }
   return fp;
}

/* End of bme280Handlers.h */

#endif
