#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

// CODE WRITTEN BY: BENJAMIN J NAGEL

/* The maximum number of bytes (excluding NULL) for file names. */
#define FILENAME_SIZE 256
/* The buffer size (in bytes) for the file transfer. */
#define BUFFER_SIZE 10
uint64_t htonll(uint64_t __hostlonglong); /* converts from host to newtowrk order */
uint64_t ntohll(uint64_t __netlonglong);  /* converts from network to host order*/
int checkforErrors(char *string);         /* Checks the filename for errors */

int main(int argc, char *argv[])
{
    /*	STREAM	SERVER	*/
    int sd;           /*	socket	descriptor	*/
    int connected_sd; /*	socket	descriptor	*/
    int rc;           /*	return	code	from	recvfrom */
    struct sockaddr_in server_address;
    struct sockaddr_in from_address;
    char buffer;                      /* buffer for filename */
    char transferbuffer[BUFFER_SIZE]; /* buffer for data to be transfered */
    socklen_t fromLength;
    FILE *output;              /* output file stream */
    char *directory = "recvd"; /* directory for the output file */

    memset(transferbuffer, 0, sizeof(transferbuffer)); /* clears the transferbuffer */
    mkdir(directory, 0777);                            /* makes the directory for the output file */

    if (argc != 2) /* checks for the correct amount of arguments */
    {
        printf("Wrong number of command line arguments\n");
        printf("Input is as follows: ftps <port-num>\n");
        exit(1);
    }

    sd = socket(AF_INET, SOCK_STREAM, 0); /* creates the socket */
    if (sd < 0)
    {
        printf("ERROR making the socket\n");
        exit(1);
    }
    printf("Socket Created\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[1]));
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind
    if (bind(sd, (struct sockaddr *)&server_address, sizeof(server_address))) // why does bind fail randomly?
    {
        printf("ERROR when binding\n");
        exit(1);
    }
    printf("Bind complete\n");

    // wait for a client connection
    if (listen(sd, 5))
    {
        printf("ERROR when listening\n");
        exit(1);
    }

    while (1)
    {
        printf("~Waiting for a connection~...\n");
        uint64_t totalbytesRead = 0;
        connected_sd = accept(sd, (struct sockaddr *)&from_address, &fromLength); /* connects to the client connection */
        if (connected_sd < 0)
        {
            printf("ERROR when accepting the connection\n");
            exit(1);
        }
        printf("Connected!\n");

        uint64_t fileSize;
        read(connected_sd, &fileSize, 8); /* gets the filesize */
        uint64_t size = ntohll(fileSize);
        printf("Size of file: %lu\n", size);

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

        if (breaked || read_in == FILENAME_SIZE || checkforErrors(filename)) /* checks for a break or if the filename is too big 
                                                                            or if there are errors in the filename */
        {
            if (breaked)
            {
                printf("ERROR: Connection to Client Has been Lost!\n");
            }
            else if (read_in == FILENAME_SIZE)
            {
                printf("ERROR: Filename Too Large!\n");
            }
            else if (checkforErrors(filename))
            {
                printf("ERROR: FOUND / or \\ in filename\n");
            }
            printf("Please try to send the file again or a different one!\n");
            continue;
        }

        char outputfile[400];
        memset(outputfile, 0, sizeof(outputfile));
        sprintf(outputfile, "recvd/%s", filename);
        printf("OutputFile name is : %s\n", outputfile); /* Open the output file */
        if ((output = fopen(outputfile, "wb")) == NULL)
        {
            printf("ERROR:Output file can not be created\n");
            exit(1);
        }

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
        if (breaked)
        {
            printf("ERROR: Connection to Client Has been Lost!\n");
            printf("Please try to send the file again!\n");
            continue;
        }
        // send the total amount of bytes read from the server
        uint64_t totalRead = htonll(totalbytesRead);
        if (ntohll(totalRead) == size)
        {
            printf("Total bytes read: %lu \n", ntohll(totalRead));
            send(connected_sd, &totalRead, sizeof(uint64_t), 0);
            break;
        }
    }
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
}


int checkforErrors(char *string) /* checks the filename for / or \\ characters. */
{
    int error = 0;
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        if ((string[i] == '\\' || (string[i] == '/')))
        {
            error = 1;
        }
    }
    return error;
}

uint64_t htonll(uint64_t __hostlonglong) /* converts from host to network byte order */
{
    return htobe64(__hostlonglong);
}

uint64_t ntohll(uint64_t __netlonglong) /* converts from network to host byte order */
{
    return be64toh(__netlonglong);
}