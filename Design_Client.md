# FTP Client Design
> This is the design document for the FTP Client ([ftpc.c](https://github.com/CSE-5462-Spring-2021/assignment-2-conner-n-ben/blob/main/ftpc.c)).  
> By: Conner Graham

## Table of Contents
- FTP Class Protocol - [Protocol Document](https://docs.google.com/document/d/1rzymcjcKfk4wPUCbcip06dY5W5nYn0OcE_5ex671D1s/edit?usp=sharing)
- [Environment Constants](#environment-constants)
- [High-Level Architecture](#high-level-architecture)
- [Low-Level Architecturet](#low-level-architecture)

## Environment Constants
```C#
NUM_ARGS = 4                    // number of command line arguments
INVALID_FILENAME_CHARS = "/\\"  // invalid characters for filename
FILENAME_SIZE = 255             // maximum filename size (excluding NULL) in bytes
MAX_FILE_SIZE = (2^64)-1        // maximum file size in bytes
BUFFER_SIZE = TBD               // buffer size in bytes for the file transfer
```

## High-Level Architecture
TODO
```C
int main(int argc, char *argv[]) {

}
```

## Low-Level Architecture
TODO

