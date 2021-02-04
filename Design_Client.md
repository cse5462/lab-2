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
    if (!correct) exit(EXIT_FAILURE);
    extractArgs(params...);
    /* open/validate file to transfer */
    if (!valid) exit(EXIT_FAILURE);
    create_endpoint(params...);
    /* establish connection to server */
    if (connected) {
        transfer_file(params...);
        /* terminate connection to server */
    }
    return 0;
}
```

## Low-Level Architecture
```C
void extractArgs(params...) {
    /* extract and validate remote IP address */
    if (!valid) exit(EXIT_FAILURE);;
    /* extract and validate remote port number */
    if (!valid) exit(EXIT_FAILURE);
    /* extract and validate filename */
    if (!valid) exit(EXIT_FAILURE);
}
```

```C
int create_endpoint(params...) {
    /* attempt to create socket */
    if (created) {
        /* initialize socket with params from user */
    } else {
        exit(EXIT_FAILURE);
    }
    return socket-descriptor;
}
```


```C
void transfer_file(params...) {
    if (transfer_header(params...)) {
        if (transfer_data(params...)) {
            /* wait to receive ACK from server */
            /* validate ACK */
        }
    }
}
```
-
    ```C
    int transfer_header(params...) {
        /* send file size to server */
        if (!sent) return FALSE;
        /* send filename (including NULL) to server */
        if (!sent) return FALSE;
        return TRUE;
    }
    ```
-
    ```C
    int transfer_data(params...) {
        while (/* successful and there is data to send */) {
            /* read data from file */
            if (error) break;
            /* send data to server */
            if (error) break;
            /* validate: bytes read from file = bytes sent to server */
        }
        /* validate: total bytes sent to server = file size */
        return (whether data sent successfully);
    }
    ```
