This is the README file for Lab_2

NAME: Conner Graham, Ben Nagel
DATE: 02/04/2021

**************
| FTP CLIENT |  By: Conner Graham
**************
DESCRIPTION:
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

USAGE: ($ denotes the command prompt)
Start the file transfer client with the command...
$ ftpc <remote-IP> <remote-port> <local-file-to-transfer>

If any of the argument strings contain whitespace, those
arguments will need to be enclosed in quotes.

ASSUMPTIONS:
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

**************
| FTP SERVER |  By: Ben Nagel
**************
DESCRIPTION:
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

USAGE: ($ denotes the command prompt)
Start the file transfer server with the command...
$ ftps <local-port>

If any of the argument strings contain whitespace, those
arguments will need to be enclosed in quotes.

ASSUMPTIONS:

- It is assumed that the filesize will be sent in one go.
- It is assumed that the file name uses and ASCII
  encoding.
- It is assumed that the file size (in bytes) will be <=
  the max UNSIGNED LONG value (i.e. (2^64)-1).
- It is assumed that the OS supports file sizes (in
  bytes) up to the max UNSIGNED LONG value.
- It is assumed that all numeric values received from the
  client will be in network order.
