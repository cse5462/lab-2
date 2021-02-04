# FTP Server Design
> This is the design document for the FTP Server ([ftps.c](https://github.com/CSE-5462-Spring-2021/assignment-2-conner-n-ben/blob/main/ftps.c)).  
> By: Ben Nagel

## Table of Contents
- FTP Class Protocol - [Protocol Document](https://docs.google.com/document/d/1rzymcjcKfk4wPUCbcip06dY5W5nYn0OcE_5ex671D1s/edit?usp=sharing)
- [Environment Constants](#environment-constants)
- [High-Level Architecture](#high-level-architecture)
- [Low-Level Architecturet](#low-level-architecture)

## Environment Constants

/* The maximum number of bytes for file names. */
#define FILENAME_SIZE 256

/* The buffer size (in bytes) for the file transfer. */

#define BUFFER_SIZE 10

## High-Level Architecture
At a high level, the server application attempts to validate and extract the arguments passed to the application. It then attempts to create the server. If everything was successful, the client will be able to connect and send over the file. Once the file has been successfuly transfered the server returns a message starting that it was a successful transmission.

## Low-Level Architecture
TODO
