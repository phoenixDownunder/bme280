#
# Makefile for BME280 June 2025
# Use at your Own Risk.
# User will be entirely responsible for any and all damage to your systems.
#
# storage size of ‘sa’ isn’t known: struct sigaction sa -D_GNU_SOURCE fixes this
CC=gcc
CFLAGS=-I$$HOME/headers/ -Wall -pedantic -std=c2x -D_GNU_SOURCE
LDLIBS=-li2c -lm

SOURCES=$(wildcard *.c)
BINARIES=$(SOURCES:%.c=%)
HEADERS=$(shell echo *.h)

default: help
distclean: help
install: help
setup: help
.PHONY: help

show:
	@echo "The source files to be built...";
	@echo " $(SOURCES:%=%\n)";
#	@echo "$(BINARIES)";

all: $(BINARIES)

%: %.c $(HEADERS)
	$(CC) -o $@ $< $(CFLAGS) $(LDLIBS)

%.o: %.c $(HEADERS)
	$(CC) -o $@ $< $(CFLAGS) $(LDLIBS)

clean:
	@echo "Removing *.o..."; rm -f *.o

cleanall:
	@echo "Removing *.o and binary files..."; rm -f *.o; rm -f $(BINARIES)

help:
#	@clear;
	@echo "Help Screen... make options...";
	@echo ""
	@echo "make all        - builds all .c files in this directory.";
	@echo "make <filename> - builds the single <filename> (ignore the .c suffix)";
	@echo "make clean      - remove all .o in the current directory.";
	@echo "make cleanall   - remove all .o and BINARIES in the current directory.";
	@echo "make show       - shows all the .c files to be built in the current directory.";
	@echo ""
	@echo "There is no make distclean, make install or make setup.";
	@echo "The executables are created in this folder and not installed elsewhere.";
	@echo ""
	@echo "If used, enable the i2c devices using \"tcaA9548a [debug] 0xNN where NN = 00..FF\".";
	@echo ""
