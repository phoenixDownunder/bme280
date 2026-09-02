/* i2cConvenientBits.h */

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
 * i2cConvenientBits.h
 *
 * August 2024.
 * Amended August 2026.
 *
 * Delay routines are used for timing.
 * Two versions - uDelay and mDelay. Effectively the same. 
 * Used for icm20948, AK09916.
 * Mostly eliminated in SSD1306 but see the SSD1306 reset command.
 * Also used when reading the tmp117 status register.
 * usleep has been deprecated.
 *
 * Contains PROTOTYPES and Function Pointers and other convenient bits that
 * are not directly part of the i2cMaster.h and i2cDeviceheader.h files...
 */

#ifndef I2C_CONVENIENT_BITS_H
#define I2C_CONVENIENT_BITS_H

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Global bits and defines.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

#define abs_val(x) ((x >= 0) ? x : 0 - x)
#define sq(x) (x*x)

char * p; /* Yes, this pointer is a global. */
 
/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * External procedures and prototypes.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

extern struct timespec ts;
extern int nanosleep(const struct timespec * duration, struct timespec * remaining);
int snprintf(char * s, size_t n, const char * format, ...);

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * Main procedures.
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

/* Two convenient Delay or Sleep routines using nanoSleep below. */ 

void uDelay(int uS); /* microSecond */
void mDelay(int mS); /* milliSecond */

/*
 * String concatenate function with minimal testing or checking.
 * From others.
 * In real code you would check for errors in malloc here
 * In this code, technically, only need a character string of length...
 * 16 x 2 + 16(spaces) + 1(initial space) + 1 = 50 ish characters
 * +1 for the null-terminator.
 * Could hard code a fixed array size, but fun doing it this way
 * Convenience routine only, used for Slave Address detail display
 */
char * concat(const char * s1, const char * s2);

/* A slightly constrained but convenient version of fgets or getline.
 * Simple but effective for these applications. Returns str length.
 */
int __getLine(FILE * fp, char * s, int limit);

/* 2 convenience WORD routines as per toupper and tolower ... ctype.h */
char * toLower(char * s);

char * toUpper(char * s);

/* Used primarily in _icm20948AK09916RecoverInitialConfig.h procedures cf isdigit(c) */
int isAnInteger(const char * str, const int len);

/* remove_blanks in CLib pg 78. This one replaces spaces with _ characters */
void replace_spaces(char * s);

/* --- --- --- Common Routines for ACCEL, GYRO and PCA9685 --- --- --- */

/* A prototype declaration double roundf(double x) to stop an annoying C89 warning */
double round(double x);

/* Round value to one decimal point...  0.1. */
double _round1DP(double a);

/* Round value to two decimal points... 0.01. */
double _round2DP(double a);

/* Round angle to 0.1 degrees... resolution should be more than enough */
double _roundAngle(double a);

/* Round Magnitudes to 2 decimal places... resolution should be more than enough */
double _roundMag(double a);

/* --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
 * .c
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- */

/* SIGNALS... standard signals only...
 * signalname(sig),  strsignal(sig)
 * sigdescr_np(sig), sigabbrev_np(sig) and sys_siglist[sig])
 * deprecated in Raspbian...
 * Created my own for convenience only.
 * Only x86/ARM signals as defined are used. Not PA-RISC.
 * char * signalname(int sig);
 * char * strsignal(int sig);
 * ...are defined.
 *
 * On error, return the empty string "" rather than NULL.
 */

char * signalname(int sig) {
   char *signame[]={"INVALID", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL",
        "SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE", "SIGKILL",
        "SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM",
        "SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP",
        "SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU",
        "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO",
        "SIGPWR", "SIGSYS", NULL};
   return ((sig > 0) & (sig < 32)) ? signame[sig] : "";
}

char * strsignal(int sig) {
   char *sigdescr[] = {"INVALID", "HANGUP", "INTERRUPT", "QUIT",
        "ILLEGAL INSTRUCTION", "TRACE/BREAKPOINT TRAP", "ABORTED",
        "BUS ERROR", "FLOATING POINT EXCEPTION", "KILLED",
        "USER DEFINED SIGNAL 1", "SEGMENTATION FAULT",
        "USER DEFINED SIGNAL 2", "BROKEN PIPE", "ALARM CLOCK",
        "TERMINATED", "STACK FAULT", "CHILD EXITED", "CONTINUED",
        "STOPPED (SIGNAL)", "STOPPED",
        "STOPPED (TTY INPUT)",
        "STOPPED (TTY OUTPUT)", "URGENT I/O CONDITION",
        "CPU TIME LIMIT EXCEEDED", "FILE SIZE LIMIT EXCEEDED",
        "VIRTUAL TIMER EXPIRED", "PROFILING TIMER EXPIRED",
        "WINDOW CHANGED", "I/O POSSIBLE", "POWER FAILURE",
        "BAD SYSTEM CALL", NULL};
   return ((sig > 0) & (sig < 32)) ? sigdescr[sig] : "";
}

/* nanosleep is POSIX. Pain in the neck. Prototype above used to get it. 
 * extern int nanosleep(const struct timespec *, struct timespec *);
 * long long for C23
 */

void __nanoDelay(time_t sec, long nS) { /* long long for C23 */
   time_t _Sec;
   long _nS;

   _Sec = (sec > 0) ? sec : -sec; /* -ve values shouldn't exist anyway. */
   _nS  = (nS  > 0) ? nS  : -nS;

   while (_nS > 999999999) {_nS = _nS - 1000000000; _Sec++; }
   ts.tv_sec  = _Sec;
   ts.tv_nsec = _nS;

/* nanosleep can and does get interrupted with the other Timeout handlers...
 * The do.. while loop handles any EINTR and allows the timer to complete normally.
 *
 * int ret;
 *    do {
 *       ret = nanosleep(&ts, NULL);
 *    } while (ret && errno == EINTR);
 */
   while ((nanosleep(&ts, NULL)) && errno == EINTR); /* EINTR is errno */
   return;
}

void uDelay(int uS) { __nanoDelay(0, (1000 * ((uS > 0) ? uS : -uS))); }

/* The maximum granular delay is in the bme688 GASMAN_MAX_WAIT_TIME or 4032mS.
 * After that, the general sleep function should suffice. Use >2 seconds.
 */ 
void mDelay(int mS) {
   long _mS = (mS > 0) ? mS : -mS;
   time_t _Sec = 0;
   while (_mS > 999) {_mS = _mS - 1000; _Sec++; }

   if (_Sec > 2) sleep(_Sec); else __nanoDelay(_Sec, (1000000 * _mS));
   return;
}

char * concat(const char * s1, const char * s2) {
   char * result = malloc(strlen(s1) + strlen(s2) + 1);
   if (result == NULL) return NULL;
   strcpy(result, s1);
   strcat(result, s2);
   return result;
}

int __getLine(FILE * fp, char * s, int limit) { /* Not error proof. */
   int i = 0;
   char c;
   for (i = 0; i < (limit - 1) && ( (c = getc(fp)) != EOF ) && (c != '\n'); )
      s[i++] = c;
   if ((c == '\n') | (c == '\0')) {
      s[i] = '\0'; /* remove \n's as well */
      return i;
   }
   return -1; 
}

char * toLower(char * s) {
   for (p = s; *p; p++) *p = tolower(*p);
  return s;
}

char * toUpper(char * s) {
  for (p = s; *p; p++) *p = toupper(*p);
  return s;
}

int isAnInteger(const char * str, const int len) {
   int i = 0;
   for (i = 0; (i < len); i++) {
      if ((str[i] == '\0') | (str[i] == '\n')) return 1; /* Includes empty str */
      if (!isdigit(str[i])) return 0;
   }
   return 1;
}

/* remove_blanks in CLib pg 78. This one replaces spaces with _ characters */
void replace_spaces(char * s) {
   char * d = s;
   do {
      while (*d == ' ') {
             *s = '_';
             ++d; ++s;
      }
   } while ((*s++ = *d++));
}

/* --- --- --- Common Routines --- --- --- */

/* A prototype declaration double roundf(double x) to stop an annoying C89 warning */
double round(double x);

/* Round value to one decimal point...  0.1. */
double _round1DP(double a) { return (round((a * 10.0))) / 10.0; }

/* Round value to two decimal points... 0.01. */
double _round2DP(double a) { return (round((a * 100.0))) / 100.0; }

/* Round angle to 0.1 degrees... resolution should be more than enough */
double _roundAngle(double a) { return _round1DP(a); }

/* Round Magnitudes to 2 decimal places... resolution should be more than enough */
double _roundMag(double a)   { return _round2DP(a); }

/* A simple str to hex check. */
int isHex(const char *hexStr) {
int i;
char c;
    for (i = 0; i < strlen(hexStr); i++) {
        c = hexStr[i];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
            return 0;
        }
    }
    return 1; /* All characters are valid */
}

#endif
