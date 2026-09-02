
// gcc -o bme280 bme280.c -I$HOME/headers/ -li2c -lm

#define BME280

#if __has_include("i2cIncludes.h")
#include "i2cIncludes.h"
#endif

#include <signal.h>

#include "bme280DataStructures.h"
#include "i2cCommonSharedMemoryDataStructure.h"
#include "i2cSharedMemoryMaster.h"
#include "bme280sharedMemoryMaster.h"
#include "bme280Handlers.h"

int _sortCommandLineArguments(int argc, char ** argv) {
   __u16 value = 0;

/* Use s = *++argv for convenience... if ( (*++argv)[0] == '-' )... */
   char * s;
/* Replacements for argc and argv */
   int _argc = argc;
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
/* options */
         switch (*s) {

         case 'n' :
                  --_argc;
                  checkArgc(_argc, pv);
                  v = (*++pv);
                  g_loop = 1;
                  if (strlen(v) > 2) { printf("Count argument %s too long\n", v); close(fp); exit(0); }
                  value = strtol(v, NULL, 10);
                  if (value == 0) g_loop = 0;
                  if ((value > 1) & (value < 1000)) g_loop = value;
                  break;
         default: break;
            }
      }
   }
   return 0;
}

int main(int argc, char **argv) {
   signal(SIGINT, intHandler);
   setlocale(LC_ALL, "");

/* Connect to or CREATe Shared memory... */
   ptr_common_shmMap = connectToSharedMemory(shmCName);
   if (ptr_common_shmMap == NULL) {
      printf("Unable to connect to shared memory, exiting.\n");
      exit(0);  /* Just in case. */
   }
   ptr_common_aMap = &common_aMap;

   printf("\n");
   printf("Use readBME280shm -n 0 run this task continuously (cycle time 1Hz).\n");
   printf("Use readBME280shm [-n 1] for a single cycle.\n");
   printf("Use stop2 or ^C for a clean interrupt...\n");
   printf("\n");

   g_quit = 0;
   g_loop = 1;
   _sortCommandLineArguments(argc, argv);

   while (!g_quit) {
      readBME280DataFromSharedMemory(ptr_common_shmMap, ptr_common_aMap);
      printf("Temperature: %.4lf%c%cC, Pressure: %.2lfhPa, Humidity: %.2f%%\n",
         common_aMap.shm.temperature, 0xC2, 0xB0, common_aMap.shm.pressure, common_aMap.shm.humidity);
      if (g_loop == 1) g_quit = 1;
      if (g_loop  > 1) g_loop--;
      if (g_quit == 0) mDelay(1000); /* Wait 1000mS */
      common_aMap.shm.terminateProcess =
         readshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.terminateProcess); 
      if (common_aMap.shm.terminateProcess) g_quit = 1;
   }
   writeshmByte(ptr_common_shmMap, ptr_common_aMap, &common_aMap.shm.terminateProcess, 0x00);
   printf("\nTerminating.\n");
   exit(0);
}
