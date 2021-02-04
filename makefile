# Makefile for Lab 2

# Setup for system
# Macros for C compiler and flags
CC = gcc
CFLAGS = -g -Wall

# Process to build application
all: ftps ftpc

ftps: ftps.c
	$(CC) $(CFLAGS) -o $@ $^

ftpc: ftpc.c
	$(CC) $(CFLAGS) -o $@ $^

# Target to open lab source code files
openCode: README.md makefile ftps.c ftpc.c
	code $^
	
# Target to open lab files
openData: foo.jpg
	code $^

# Remove executables for clean build
clean:
	rm ftps ftpc
