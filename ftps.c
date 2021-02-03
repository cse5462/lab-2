#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
/* The maximum number of bytes (excluding NULL) for file names. */
#define FILENAME_SIZE 255
/* The buffer size (in bytes) for the file transfer. */
#define BUFFER_SIZE 10
uint64_t htonll(uint64_t __hostlonglong);
uint64_t ntohll(uint64_t __netlonglong);
int checkforNullTerminate(char *string);
int checkforErrors(char *string);
int main(int argc, char *argv[])
{
    /*	STREAM	SERVER	*/
    int sd;           /*	socket	descriptor	*/
    int connected_sd; /*	socket	descriptor	*/
    int rc;           /*	return	code	from	recvfrom */
    struct sockaddr_in server_address;
    struct sockaddr_in from_address;
    char buffer;
    char transferbuffer[BUFFER_SIZE];
    socklen_t fromLength;
    FILE *output;
    char * directory="recvd";
    //Question should this be a uint64_t?
    memset(transferbuffer, 0, sizeof(transferbuffer));

    mkdir(directory,0777);
    // check for two arguments
    if (argc != 2)
    {
        printf("Wrong number of command line arguments\n");
        printf("Input is as follows: ftps <port-num>\n");
        exit(1);
    }
    // create the socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        printf("ERROR making the socket");
        exit(1);
    }
    printf("Socket Created\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[1]));
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind
    if (bind(sd, (struct sockaddr *)&server_address, sizeof(server_address))) // why does bind fail randomly?
    {
        printf("ERROR when binding");
        exit(1);
    }
    printf("Bind complete\n");

    // wait for a client connection
    if (listen(sd, 1))
    {
        printf("ERROR when listening");
        exit(1);
    }
    printf("~Waiting for a connection~...\n");
    while(1){
         uint64_t totalbytesRead = 0;
    // connect to the client connection
    connected_sd = accept(sd, (struct sockaddr *)&from_address, &fromLength);
    if (connected_sd < 0)
    {
        printf("ERROR when accepting the connection");
        exit(1);
    }
    printf("Connected!\n");
    //reads the file size
    int read_in = 0;
    uint64_t fileSize;
    read(connected_sd, &fileSize, 8);
    // change to network or host order
    uint64_t size = ntohll(fileSize);
    printf("Size of file: %lu\n", size);
    //reads the file names
    char filename[FILENAME_SIZE];
    memset(filename, 0, sizeof(filename));
    int x = 0;
    int nullFound = 0;
    int breaked=0;
    do
    {
        rc = read(connected_sd, &buffer, 1);
         if(rc<=0){
             breaked=1;
            break;
        }
        read_in += rc;
       // printf("char read in is : %x\n",buffer);
        if (buffer != 0)
        {
            filename[x] = buffer;
            //printf("Filename is: %s\n",filename);
        }
        else
        {
            nullFound = 1;
        }
        x++;
        
    } while (!nullFound && read_in < FILENAME_SIZE);
    // checks to see if the null terminator is there
    //printf("nullFound : %d  and checkforErrors : %d\n",nullFound,checkforErrors(filename));
    if(breaked){
        printf("ERROR: Connection to Client Has been Lost!\n");
        printf("Please try to send the file again!\n");
        continue;
    }
    if (!nullFound || checkforErrors(filename))
    {
        printf("got in if \n");
        close(connected_sd);
        exit(1);
    }
    //printf(" after if\n");
    // open the output file
    char outputfile[400];
    memset(outputfile, 0, sizeof(outputfile));
    sprintf(outputfile, "recvd/%s", filename);
    printf("OutputFile name is : %s\n", outputfile);
    if ((output = fopen(outputfile, "wb")) == NULL)
    {
        printf("ERROR:Output file can not be created\n");
        exit(1);
    }
 
    // read from the stream till the end of the stream
    do
    {
        rc = read(connected_sd, &transferbuffer, 10);
         if(rc<=0){
             breaked=1;
            break;
        }
        fwrite(transferbuffer,rc,1,output);
        totalbytesRead += rc;
        //printf("Inside transferbuffer: %s\n",transferbuffer);
        bzero(transferbuffer, BUFFER_SIZE);
    } while (totalbytesRead < size);
     if(breaked){
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

// checks the filename for invalid characters
int checkforErrors(char *string)
{
    int error=0;
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

uint64_t htonll(uint64_t __hostlonglong)
{
    return htobe64(__hostlonglong);
}

uint64_t ntohll(uint64_t __netlonglong)
{
    return be64toh(__netlonglong);
}
