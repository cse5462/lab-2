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
BUFFER_SIZE = TBD               // buffer size in bytes for the file transfer
```

## High-Level Architecture
At a high level, the client application attempts to validate and extract the arguments passed
to the application. It then attempts to validate the file and create the server endpoint. If
everything was successful, the client then attempts to establish a connection to the server.
If a connection is able to be established the client transfers the specified file to the server
and closes the connection.
```C
int main(int argc, char *argv[]) {
    /* check that the arg count is correct */
    if (correct) extractArgs(params...);
    /* open/validate file to transfer */
    create_endpoint(params...);
    /* establish connection to server */
    if (connected) {
        transfer_file(params...);
        /* terminate connection to server */
    }
}
```

## Low-Level Architecture
TODO

