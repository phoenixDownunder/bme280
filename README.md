# README.md

* This document.
* Last update date September 2026.

--
# BME280 Sensor

* The bme280 is a simple Temperature, Humidity and Pressure sensor on a single chip.

* The BME280 version used here is the SparkFun Qwiic BME280-Environmental-Sensor-Breakout. See the image at [SparkFun BME280](SparkFun-©-BME280-_Environmental_Sensor_Breakout.jpg)

--
# Contents

* BME280 Sensor
* Contents
* Device Reference
* Raspberry Pi and i2c
* Licence
* Disclaimer
* The bme280Handlers
* Quick Build
* Quick Remove
* Common Shared Memory
* Common Shared Memory Simple Utilities
* Keeping It Simple
* Preparation
* Fast i2c interface
* Basic BME80 Operation
* Additional i2c Support
* Author

--
# Device Reference

* The current reference document for the BME280 is...
 * BST-BME280-DS001-24
 * Revision 1.24 February 2024.
 * [bme280](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf) pdf document link as of September 2026.

--
# Raspberry Pi and i2c

* Raspberry Pi: A Raspberry Pi is used to manage the bme280 along with a range of other i2c connected devices. Really only tested with RPi4 (1Gb, 4Gb and 8Gb) and RPi5. Untested with RPi3 but should work.

* Interface: The interface is 400kb i2c (rather than SPI) and the Sparkfun version which is being used for testing, has a default i2c address 0x77. See separate section below.

* Common Shared Memory: All attached (i2c) devices communicate via shared memory. See separate section below.

* Load: On a Raspberry Pi 4 and using Raspbian Lite (Trixie at this time), the software runs flat out and the RPi is mostly bored.

* Descriptors: All i2c devices attached are described in the header file i2cDevicesHeader.h. The active device is defined in the **Main** routine, in this case, \#define BME280 . If any of the header files are altered, then the whole application should be re-compiled. See separate section below. The current i2cDevicesHeader.h file has been cut back and is basically static. New experimental device are attached occasionally.

--
# Licence

This software is released under the [BSD-3-Clause Licence](Licence.md) and a copy of the licence is provided at the top level. A copy of the said licence has been placed in each of the source files to satisfy the legal Dementors but some files, for example, the Unit Test files, are working files and therefore clean. 

--
# Disclaimer

This is a development framework and experimental software. It mostly works as designed. Hopefully there aren't too many bugs. Use at your own risk. No liability. If it crashes your Raspberry Pi (highly unlikely) or blows up your cat (more likely), it's your problem.

--
# Quick build

* Prepare a Raspberry Pi(4+) with the i2c libraries (See Preparation below) in the usual way.
* Copy the bme280.d folder to the **HOME** directory.
* Copy or move the headers folder to the **HOME** directory.
* To build the bme280 executable, run
 
```
cd bme280.d
make cleanall
make all
```
This is equivalent to

```
gcc -o bme280 bme280.c -I$HOME/headers/ -Wall -pedantic -std=c2x -D_GNU_SOURCE -li2c -lm
```

then run the executable with any options such as those presented below.
Set the link below for convenience if required.
The examples below terminate after operation.

```
./bme280 ?
./bme280 -v <-- Displays all registers
./bme280 -n 2
./bme280 -n 2 debug
./bme280 -n 2 normal <-- Run in NORMAL MODE. The default is FORCED_MODE.

ln -s /home/(username)/bme280.d/bme280 ~/bin/bme280
```

--
### Quick Remove
To remove the whole lot, remove the directories bme280.d and headers. Nothing is saved elsewhere.

--
# Common Shared Memory

The centre of the world is a Common Shared Memory (or SHM). All interprocess communications, initialisation and raw data storage is conducted through shared memory.
Shared memory (sometime multiple) is used with a variety of sensors and displays (including SSD1306, TCA9548, BME688, TMP117, ICM20948 and others) together with the Raspberry Pi. 

By definition, when created, there is **NO STRUCTURE to shared memory**, only a size. Shared memory is just an allocated blob of run\-time main memory.

A common (union) data structure, in this case named **common_shmMap**, is logically overlay-ed on the shared memory. **All processes using that shared memory must use the same definition**. It can be a lot of fun if they don't.

For these applications and related processes, a 4K byte shared memory space is created (4096 (2\*\*12) bytes treated as 32 (2\*\*5) banks of 128bytes (2\*\*7)) and initialised to 0x00.

When a variable is accessed within the shared memory, it is simply an implicit length in bytes, a pointer to the variable together with a run\-time pointer to the shared memory. For example, int shm.temperature is a location starting at the (runtime pointer) shm address and offset address to the location of shm.temperature and going for 4 bytes. A double is 8 bytes. The structure of any variable is system dependent. This is very useful.

--
#### Common Shared Memory Simple Utilities ####

To manage and view shared memory, a number of separate much smaller routines have been created. None of these routine need to be used. They are mostly informational. The shmC... functions are typically generic shared memory functions while the shmCbme280_... are orientated specifically to the bme280. (See also bme688)

These routines are not normally used, and some functions are replicated.

The shmC... prefix refers to use of the C common shared memory. Other shared memory versions (A and B) are not used here.

```
shmCDisplay [n] where 0 < n < 33
```
Displays common shared memory banks between 0 and 31. Really only sensible to look at the first block in this case.

```
shmCbme280DisplayResults
```
More comprehensive display of common shared memory bank 0 (only) with offsets to expected bme280 Temperature, Pressure, Humidity results.

```
stop1 [1 or 0]
```
A simple routine to shutdown properly and terminate all associated shared processes.

--


# Keeping It Simple

* With the exception of the main procedure and the shared memory utilities above, all software described is contained in the file bme280Handlers.h.
 
* Everything is written in C. The software has been written and designed to be readable and therefore sometimes a little verbose. No hidden tricks, obfuscating code or short unreadable naming tricks are used.

* Although C pointers are used extensively, each is carefully named and the paths included where necessary. The use of system level libraries and the reason for a library inclusion is explained in the i2cIncludes.h file. For example, the definition for __u8 etc. in \<linux/i2c\-dev.h\>. Other convenient routines (many no longer used or used elsewhere but remain for interest) are contained in ./headers/i2cConvenients.h.

* No separate third party libraries are used.

* The procedures in bme280Handlers.h run on the Raspberry Pi and provide a variety of ways to configure and recover sensor data from the bme280 and save the results to a **Shared Memory** system for other processes such as SSD1306 displays to access. A GUI version of Raspbian can be used but all the software below runs from the command line. Processes to recover TPHG data from shared memory and display it using gnuplots are separate.

* All procedures in bme280Handlers.h are mostly documented (as best) step by step. The procedures are designed to be a framework of routines to be used together.

* Makefiles are used to compile into a local directory. Or you can use gcc directly. The c2x standard is used. C11 will show comment errors. No files are written into system areas. Several i2c related system libraries are preinstalled as described in the **Preparation** below.

* The most important variable is of course the device file pointer, **fp**. If the device is not found, everything stops.

--
# Preparation

* The first and most obvious stage is to prepare a Raspberry Pi.
* Any version can be used but a RPi4 or faster with at least 1Gb of RAM is suggested. I haven't tried RPi3 or RPiZero. An RPi5 won't function any better than the RPi4 because of the bme688 design in this case.
* A reasonably fast SDRAM card 16Gb or better is suggested.
* Download a recent 64bit version of Raspbian Lite, currently Trixie, in the usual way and prepare the SDRAM. Strictly speaking, only Raspbian Lite is required but you can install a GUI version if you so desire. On a Mac Terminal screen, run

```
sudo dd bs=16m if=./2026-06-18-raspios-trixie-arm64-lite.img of=/dev/rdisk2
```

* or use balenaEtcher or similar.
 
* Install and boot the RPi.

```
sudo dpkg-reconfigure console-setup
```

* (For example, try UTF-8 -> Guess -> Terminus -> 12x24 framebuffer... to set console font size from the default. Works for 27" and 32" monitors)

* Once started, log in and run the command line routine...

```
sudo raspi-config
```

* to set the host name (for example RPi44), wifi connection, enable interface->i2c and interface->ssh. All other options are you own. Reboot as required.
Run the following command (or ip -4 a (enter)) to find your local ip address for a local ssh connection and then complete all sensible updates...

```
ifconfig
```

```
sudo apt update
sudo apt upgrade -y
sync
sudo apt dist-upgrade
sudo apt autoremove
sync
```

* Create /etc/sudoers.d/010_pi-nopasswd and put in... (yes I know that the username pi shouldn't be used but its convenient!)

```
pi ALL=(ALL) NOPASSWD: ALL  
```
```
sudo shutdown -r now  
```

* Reconnect and...

```
sudo apt install i2c-tools  
sudo apt-get install libi2c-dev   
sudo apt-get install samba  
```

* Samba is an optional convenience. It can be used to transfer files between Apple Macs (both High Sierra and Tahoe) and other Linux systems. Everything is relatively private and fast. In /etc/samba/smb.conf, my global options are...

```
[global]
   workgroup...
   
------- LOCAL ADDITION -------
security = user
encrypt passwords = true
# smb v4.13 or earlier
smb encrypt = required
unix extensions = no
allow insecure wide links = yes
veto files = /._*/.DS_Store/
------- LOCAL ADDITION -------
```

* and further down in the Share Definitions section...
(If you like, disable [homes] and all associated options.
Simplifies specifically named shares across multiple RPis)

```
[RPi5Trixie]       <--- arbitrary name for example...
    comment = pi profiles  
    path = /home/pi  
    guest ok = yes
    writeable = yes
    browseable = yes
    create mask = 0644]
    directory mask = 0755
    follow symlinks = yes
    wide links = yes
```

* Once you have Samba working, set the smb password(s) for the user(s), pi in this case...

```
sudo smbpasswd -a pi (enter)
```

--
# Fast i2c interface...

* Now, to use a fast i2c interface, edit the file /boot/firmware/config.txt and add...

```
# i2c  
dtparam=i2c=on,i2c_baudrate=400000
```

* There appears to be whole pile of variations of this exercise to achieve the same result. This works at the moment (Trixie 2026) but check.

--
# Basic bme280 Operations...

* The Individual Command Line format (in BNF with [ ]s) is...

```
bme280 [debug] [nodisplay] [-a adapterAddress][-b slaveAddress] [-d][-D][f][-h][-m][-p][-s][-t] [0,1,2,4,8(default),16] [-v] [-n nn] [normal]
```

* -h : set over sampling rate 1x 2x 4x 8x 16x for humidity sensor in shared memory, then terminate.
* -p : set over sampling rate 1x 2x 4x 8x 16x for pressure sensor in shared memory, then terminate.
* -t : set over sampling rate 1x 2x 4x 8x 16x for temperature sensor in shared memory, then terminate.
* -f : set the IIR Filter Coefficient 0x 2x 4x 8x 16x in shared memory, then terminate.
* -d : set default osrs for all sensors in shared memory, then terminate.
* -D : set minimum osrs for all sensors in shared memory, then terminate.
* -s : set standby time mS or Seconds for NORMAL mode in shared memory, then terminate.
   : Use 0.5(mS), 62.5, 125, 250, 500, 1000(mS) or 1(s), 10, 20

```
bme280 -n nn
```

* -n : nn set loop mode [0 for infinite or nn seconds] and proceeds to the runLoop
* -m : set mode of operation, Off[0], Forced[1](default), Normal[3] runLoop
* Use bme280 -n 0 to run continuously (Forced mode with cycle time 1Hz)...   
* -v : Display all current device registers, then terminate.
* -? : Display this help summary
* ?  : Display this help summary  

```
bme280 [normal] (enter)
```  

* The bme280 has 3 modes of operation...
  * OFF\_MODE... (ignore)
  * FORCED\_MODE... This is the default operating mode. 
  * NORMAL\_MODE... Normal Mode, control is maintained by the RPi.

```
bme280 (enter)
```

*  loop_count is 1 (default).
*  loop_time is 1000mS(default).
* A variation of the above command example would be...
 
```
bme280 [-n 2] [-s 0] [debug] (enter)
```

* Arguments are processed in argument order. All other arguments are ignored
* Use bme280 debug : run in DEBUG mode.
* Use bme280 nodisplay : to stop showing the results as they are received. Final results are shown.
* Use stop2 or ^C for a controlled interrupt...
  
```
bme280 -n 2 (enter) will display...
Temperature: 18.38°, QNH: 1025.61hPa, Humidity: 60.85%, Dew Point: 10.69°
Temperature: 18.37°, QNH: 1025.61hPa, Humidity: 60.82%, Dew Point: 10.67°
```

--
# Additional i2c Support...
 
* Use ``` i2cdetect -y 1 ``` to find the i2c device

* Use ``` bme280 -v ``` to see ALL the current device registers. The green registers are the active static registers.

* There are other i2c scanning (i2cDetect) routines. See other sources.

--

--
# End Statement

There are a huge number of operating options for the bme688 and hopefully they are covered in the settings. Use at your own risk. Hopefully, there won't be too many bugs and not too many errors in this document!

--
# Author

* Author: Bruce Stephens.
* Country of origin: Australia.
* Most Recent Update: September 2026.
* Version Number: soon...
* Errors : bruce @ eigenspaces.com.au

--