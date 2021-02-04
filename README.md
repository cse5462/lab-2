# File Transfer Program – Stream Sockets
> This is the README file for [Lab_2](https://osu.instructure.com/courses/97443/files/27903220/download?download_frd=1)

**NAME:** Conner Graham, Ben Nagel  
**DATE:** 02/04/2021

## Table of Contents:
- [Included Files](#included-files)
- [FTP Client](#ftp-client)
  - [Description](#description-client)
  - [Usage](#usage-client)
  - [Assumptions](#assumptions-client)
- [FTP Server](#ftp-server)
  - [Description](#description-server)
  - [Usage](#usage-server)
  - [Assumptions](#assumptions-server)

## Included Files
- [makefile](https://github.com/CSE-5462-Spring-2021/assignment-2-conner-n-ben/blob/main/makefile)
- Client Design Document - Design_Client.md
- FTP Client Source Code - [ftpc.c](https://github.com/CSE-5462-Spring-2021/assignment-2-conner-n-ben/blob/main/ftpc.c)
- FTP Server Source Code - [ftps.c](https://github.com/CSE-5462-Spring-2021/assignment-2-conner-n-ben/blob/main/ftps.c)

## FTP CLIENT
> By: Conner Graham

### DESCRIPTION <a name="description-client"></a>
This lab contains a program called "ftpc" which creates
and sets up a file transfer protocal client. This client
connects to the specified server (IP address and port),
reads a file (in chunks), and transfers the file to the
server. The specific tasks the client performs are as
follows:
- Create server socket from user provided IP/port
- Establish TCP STREAM connection to remote server
- Perform file transfer over the connection
- Confirm file transfer success with ACK from server
- Terminate the connection to the server

If the number of arguments is incorrect, or the
local-file-to-transfer is incorrect, or the remote IP
address/port is invalid, the program prints appropriate
messages and shows how to correctly invoke the program. 

### USAGE <a name="usage-client"></a>
Start the FTP Client with the command...
```sh
$ ftpc <remote-IP> <remote-port> <local-file-to-transfer>
```

If any of the argument strings contain whitespace, those
arguments will need to be enclosed in quotes.

### ASSUMPTIONS <a name="assumptions-client"></a>
- It is assumed that each argument string retrieved from
  the command line will be NULL terminated with no other
  NULL characters present within the string.
- It is assumed that the file name uses and ASCII
  encoding.
- It is assumed that the file size (in bytes) will be <=
  the max UNSIGNED LONG value (i.e. (2^64)-1).
- It is assumed that the OS supports file sizes (in
  bytes) up to the max UNSIGNED LONG value.
- It is assumed that all numeric values received from the
  server will be in network order.
- It is assumed that the IP addresses 0.0.0.0 and
  255.255.255.255 are invalid remote server addresses to
  connect to as they are reserved values.
- It is assumed that the ACK (file size) will be sent in
  one go such that it can be read all at once.

## FTP SERVER
> By: Ben Nagel

### DESCRIPTION <a name="description-server"></a>
This lab contains a program called "ftps" which creates a server 
and sets up the server which waits to be connected with a client 
following the defined protocol. The server connects to an incoming client
reads in the filesize, filename, and file data. Once the server is done reading
from the file, it returns the amount of bytes read if that number matches 
how many bytes the client sent.

The specific tasks the client performs are as
follows:
- Create server socket from user provided port
- Establish TCP STREAM connection to remote client
- Perform file transfer over the connection
- Reset's the server if the stream is cut mid-send or any other errors.
- Returns the amount of bytes read from the client stream
- Closes the connection to the client 

### USAGE <a name="usage-server"></a>
Start the FTP Server with the command...
```sh
$ ftps <local-port>
```

If any of the argument strings contain whitespace, those
arguments will need to be enclosed in quotes.

### ASSUMPTIONS <a name="assumptions-server"></a>
- It is assumed that the filesize will be sent in one go.
- It is assumed that the file name uses and ASCII
  encoding.
- It is assumed that the file size (in bytes) will be <=
  the max UNSIGNED LONG value (i.e. (2^64)-1).
- It is assumed that the OS supports file sizes (in
  bytes) up to the max UNSIGNED LONG value.
- It is assumed that all numeric values received from the
  client will be in network order.
