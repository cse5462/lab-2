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
Reads input from the network Stream and gets the size of the file thats going to be transfered

        uint64_t fileSize;
        read(connected_sd, &fileSize, 8); /* gets the filesize */
        uint64_t size = ntohll(fileSize);
        printf("Size of file: %lu\n", size);
        
Reads input from the network stream and gets the file name.

//reads the file names
        char filename[FILENAME_SIZE];
        memset(filename, 0, sizeof(filename));
        int x = 0;
        int nullFound = 0;
        int breaked = 0;
        int read_in = 0;
        do
        {
            rc = read(connected_sd, &buffer, 1);
            if (rc <= 0)
            {
                breaked = 1;
                break;
            }
            read_in += rc;
            if (buffer != 0)
            {
                filename[x] = buffer;
            }
            else
            {
                nullFound = 1;
            }
            x++;

        } while (!nullFound && read_in < FILENAME_SIZE);

Creates the output file to write to in the recvd directory

 char outputfile[400];
        memset(outputfile, 0, sizeof(outputfile));
        sprintf(outputfile, "recvd/%s", filename);
        printf("OutputFile name is : %s\n", outputfile); /* Open the output file */
        if ((output = fopen(outputfile, "wb")) == NULL)
        {
            printf("ERROR:Output file can not be created\n");
            exit(1);
        }

 Reads from the network stream reading the file file that is being transfered while writing it
to the output file
 do
        {
            rc = read(connected_sd, &transferbuffer, 10); /* read in the transfer data from the client */
            if (rc <= 0)
            {
                breaked = 1;
                break;
            }
            fwrite(transferbuffer, rc, 1, output);
            totalbytesRead += rc;
            bzero(transferbuffer, BUFFER_SIZE);

        } while (totalbytesRead < size);


Closes the sockets when the transfer is over
// close all socket descriptors and files
    if (close(connected_sd) < 0)
    {
        printf("ERROR CLOSING Socket descriptor");
    }
    printf("Closing the connection\n");
    if (close(sd) < 0)
        printf("ERROR CLOSING Socket descriptor");
    if (fclose(output) < 0)
        printf("ERROR CLOSING Socket descriptor");
    return 0;
