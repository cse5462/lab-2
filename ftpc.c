#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <endian.h>

/* The number of command line arguments. */
#define NUM_ARGS 4
/* Characters considerend to be invalid in a filename */
#define INVALID_FILENAME_CHARS "/\\"
/* The maximum number of bytes (excluding NULL) for file names. */
#define FILENAME_SIZE 255
/* The buffer size (in bytes) for the file transfer. */
#define BUFFER_SIZE 5000

uint64_t htonll(uint64_t __hostlonglong);
uint64_t ntohll(uint64_t __netlonglong);
void print_error(const char *msg, int errnum, int terminate);
void handle_init_error(const char *msg, int errnoSet);
void extractArgs(char *argv[], unsigned long *address, int *port, char *filename);
int create_endpoint(struct sockaddr_in *socketAddr, unsigned long address, int port);
unsigned long get_file_size(FILE *file);
int transfer_header(int sd, unsigned long fileSize, char *filename);
int transfer_data(int sd, unsigned long fileSize, FILE *file);
void transfer_file(int sd, char *filename, FILE *file);

/**
 * @brief This program creates and sets up a file transfer protocal client. This client creates
 * a server socket from user provided remote-IP and remote-port, establishes a TCP STREAM
 * connection to remote server, sends the local-file-to-transfer (in chunks) over the connection
 * and confirms success with an ACK from server, and finally terminates the connection to the
 * server. If an error occurs before the connection is established, the program terminates and
 * prints appropriate error messages, otherwise an error message is printed and the connection
 * is terminated.
 * 
 * @param argc Non-negative value representing the number of arguments passed to the program
 * from the environment in which the program is run.
 * @param argv Pointer to the first element of an array of argc + 1 pointers, of which the
 * last one is null and the previous ones, if any, point to strings that represent the
 * arguments passed to the program from the host environment. If argv[0] is not a null
 * pointer (or, equivalently, if argc > 0), it points to a string that represents the program
 * name, which is empty if the program name is not available from the host environment.
 * @return If the return statement is used, the return value is used as the argument to the
 * implicit call to exit(). The values zero and EXIT_SUCCESS indicate successful termination,
 * the value EXIT_FAILURE indicates unsuccessful termination.
 */
int main(int argc, char *argv[]) {
	int portNumber, socketDesc;
	unsigned long serverNetAddr;
	char localFilename[FILENAME_SIZE+1] = {0};
	FILE *localFile;
	struct sockaddr_in serverAddr;
	
	/* If arg count correct, extract arguments to their respective variables */
	if (argc != NUM_ARGS) handle_init_error("argc: Invalid number of command line arguments", 0);
	extractArgs(argv, &serverNetAddr, &portNumber, localFilename);
	/* Attempt to open the local file to transfer */
	if ((localFile = fopen(localFilename, "rb")) == NULL) handle_init_error("open local-file-to-transfer", errno);

	/* Create server socket from user provided data */
	socketDesc = create_endpoint(&serverAddr, serverNetAddr, portNumber);
	/* Establish connection to server and transfer file */
	if (connect(socketDesc, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) != -1) {
		printf("[+]Connected to server.\n");
		printf("Connected to IP address %s on port %hu\n", argv[1], htons(portNumber));
		/* Transfer the specified file */
		transfer_file(socketDesc, localFilename, localFile);
		/* Close file since transmission is finished */
		if (fclose(localFile) < 0) print_error("close local-file-to-transfer", errno, 0);
		/* Terminate connection to server */
		printf("[+]Closing the connection.\n");
		if (close(socketDesc) < 0) print_error("close socket-descriptor", errno, 0);
	} else {
		print_error("connect_to_server", errno, 0);
	}
	return 0;
}

/**
 * @brief Converts the unsigned long integer hostlonglong from host byte order to network byte order.
 * 
 * @param __hostlonglong 
 * @return The provided unsigned long integer in network order.
 */
uint64_t htonll(uint64_t __hostlonglong) {
	return htobe64(__hostlonglong);
}

/**
 * @brief Converts the unsigned long integer netlonglong from network byte order to host byte order.
 * 
 * @param __netlonglong 
 * @return The provided unsigned long integer in host order.
 */
uint64_t ntohll(uint64_t __netlonglong) {
	return be64toh(__netlonglong);
}

/**
 * @brief Prints the provided error message and corresponding errno message (if present) and
 * terminates the process if asked to do so.
 * 
 * @param msg The error description message to display.
 * @param errnum This is the error number, usually errno.
 * @param terminate Whether or not the process should be terminated.
 */
void print_error(const char *msg, int errnum, int terminate) {
	/* Check for valid error code and generate error message */
	if (errnum) {
		printf("ERROR: %s: %s\n", msg, strerror(errnum));
	} else {
		printf("ERROR: %s\n", msg);
	}
	/* Exits process if it should be terminated */
	if (terminate) exit(EXIT_FAILURE);
}

/**
 * @brief Prints a string describing the initialization error and provided error number (if
 * nonzero), the correct command usage, and exits the process signaling unsuccessful termination. 
 * 
 * @param msg The error description message to display.
 * @param errnum This is the error number, usually errno.
 */
void handle_init_error(const char *msg, int errnum) {
	print_error(msg, errnum, 0);
	printf("Usage is: ftpc <remote-IP> <remote-port> <local-file-to-transfer>\n");
	/* Exits the process signaling unsuccessful termination */
	exit(EXIT_FAILURE);
}

/**
 * @brief Determines whether of not the privided string is valid or not based on the set of
 * characters to reject that determine invalid strings.
 * 
 * @param str The string to validate.
 * @param reject String od characters that should be rejected.
 * @return Whether of not the privided string is valid or not.
 */
int valid_str(const char *str, const char *reject) {
	int i;
	for (i = 0; i < strlen(reject); i++) {
		/* If invalid character found, string is invalid */
		if (strchr(str, reject[i])) return 0;
	}
	return 1;
}

/**
 * @brief Extracts the user provided arguments to their respective local variables and performs
 * validation on their formatting. If any errors are found, the function terminates the process.
 * 
 * @param argv Pointer to the first element of an array of argc + 1 pointers, of which the
 * last one is null and the previous ones, if any, point to strings that represent the
 * arguments passed to the program from the host environment. If argv[0] is not a null
 * pointer (or, equivalently, if argc > 0), it points to a string that represents the program
 * name, which is empty if the program name is not available from the host environment.
 * @param address The remote IP address of the server
 * @param port The remote port number that the server is listening on
 * @param filename The filename of the local file to transfer
 */
void extractArgs(char *argv[], unsigned long *address, int *port, char *filename) {
	/* Extract and validate remote IP address */
	*address = inet_addr(argv[1]);
	if (*address == INADDR_NONE || *address == INADDR_ANY) handle_init_error("remote-IP: Invalid server address", 0);
	/* Extract and validate remote port number */
	*port = strtol(argv[2], NULL, 10);
	if (*port < 1 || *port != (u_int16_t)(*port)) handle_init_error("remote-port: Invalid port number", 0);
	/* Extract and validate the filename of the file to transfer */
	strcpy(filename, argv[3]);
	if (strlen(filename) > FILENAME_SIZE) handle_init_error("local-file-to-transfer: Filename is too long", 0);
	if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0 || !valid_str(filename, INVALID_FILENAME_CHARS)) {
		handle_init_error("local-file-to-transfer: Invalid filename or characters in filename", 0);
	}
}

/**
 * @brief Creates the comminication endpoint with the provided IP address and port number. If any
 * errors are found, the function terminates the process.
 * 
 * @param socketAddr The socket address structure creates for the comminication endpoint.
 * @param address The IP address for the socket address structure.
 * @param port The port number for the socket address structure.
 * @return The socket descriptor of the created comminication endpoint.
 */
int create_endpoint(struct sockaddr_in *socketAddr, unsigned long address, int port) {
	int sd;
	/* Create socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) != -1) {
		socketAddr->sin_family = AF_INET;
		/* Assign IP address to socket */
		socketAddr->sin_addr.s_addr = address;
		/* Assign port number to socket */
		socketAddr->sin_port = htons(port);
		printf("[+]Server socket created successfully.\n");
	} else {
		print_error("create_endpoint", errno, 1);
	}
	return sd;
}

/**
 * @brief Gets the size (in bytes) of the provided file and sets the current position to
 * the beginning of the file.
 * 
 * @param file The pointer to a FILE object.
 * @return The file size in bytes.
 */
unsigned long get_file_size(FILE *file) {
	unsigned long size;
	/* Set position to end of file */
	if (fseek(file, 0, SEEK_END) < 0) print_error("get_file_size", errno, 0);
	/* Get current byte position in file for file size */
	if ((size = ftell(file)) == -1) print_error("get_file_size", errno, 0);
	/* Reset position to beginning of file */
	rewind(file);
	return size;
}

/**
 * @brief Transfers the file header information to the server based on the established
 * protocol: | size (8 bytes) | filename | '\\0' |.
 * 
 * @param sd The socket descriptor of the server comminication endpoint.
 * @param fileSize The size of the file (in bytes) to transfer.
 * @param filename The name of the file to transfer.
 * @return Whether of not the file header information was successfully sent. 
 */
int transfer_header(int sd, unsigned long fileSize, char *filename) {
	/* Convert file fize to network order to send */
	uint64_t netFileSize = htonll(fileSize);
	/* Send the 8 bytes of file size information to the server */
	printf("Sending file size of %lu bytes\n", fileSize);
	if (send(sd, &netFileSize, sizeof(uint64_t), MSG_NOSIGNAL) < 0) {
		print_error("transfer_header", errno, 0);
		return 0;
	}
	/* Send the filename (terminated by NULL character) to the server */
	printf("Sending filename \'%s\'\n", filename);
	if (send(sd, filename, strlen(filename)+1, MSG_NOSIGNAL) < 0) {
		print_error("transfer_header", errno, 0);
		return 0;
	}
	return 1;
}

/**
 * @brief Transfers the file data to the server.
 * 
 * @param sd The socket descriptor of the server comminication endpoint.
 * @param fileSize The size of the file (in bytes) to transfer.
 * @param file The file to transfer.
 * @return Whether of not the file data was successfully sent. 
 */
int transfer_data(int sd, unsigned long fileSize, FILE *file) {
	int isSuccess = 1, bRead = 0, bSent = 0;
	unsigned long totalSent = 0;
	char buffer[BUFFER_SIZE];

	/* Transfer file data to server */
	do {
		/* Read file chunk into buffer; break if error reading from file */
		bRead = fread(buffer, sizeof(char), BUFFER_SIZE, file);
		if (ferror(file)) {
			print_error("file_read: Error reading from file", 0, 0);
			isSuccess = 0;
			break;
		}
		/* Send file chunk to server; break if error sending file */
		bSent = send(sd, buffer, bRead, MSG_NOSIGNAL);
		if (bSent > 0) totalSent += bSent;
		if (bSent < 0) {
			print_error("transfer_data", errno, 0);
			isSuccess = 0;
			break;
		}
		/* Check that the bytes read from the file is the same as the bytes sent */
		if (bRead != bSent) {
			print_error("transfer_data: Number of bytes read and sent differ", 0, 0);
			isSuccess = 0;
		}
	} while (bRead == BUFFER_SIZE);
	printf("Total of %lu bytes of data sent\n", totalSent);
	/* Check that the total bytes sent is the same as the file size */
	if (!isSuccess || totalSent != fileSize) {
		print_error("transfer_data: File data sent unsuccessfully", 0, 0);
		isSuccess = 0;
	} else {
		printf("File data sent successfully\n");
	}
	return isSuccess;
}

/**
 * @brief Transfer the file using the established protocol and check that the server
 * received it successfully.
 * 
 * @param sd The socket descriptor of the server comminication endpoint.
 * @param filename The name of the file to transfer.
 * @param file The file to transfer.
 */
void transfer_file(int sd, char *filename, FILE *file) {
	/* Get size of the file to transfer */
	unsigned long fileSize = get_file_size(file);
	/* Transfer file using established protocol */
	printf("[+]Executing file transfer.\n");
	if (transfer_header(sd, fileSize, filename)) {
		if (transfer_data(sd, fileSize, file)) {
			uint64_t bytesRecvd = -1;
			/* Wait for server to acknowledge receipt of file */
			printf("[+]Waiting for ACK from server.\n");
			if (recv(sd, &bytesRecvd, sizeof(uint64_t), 0) < 0) {
				print_error("recv_ack", errno, 0);
				return;
			} else {
				printf("Server received %lu of total %lu bytes\n", ntohll(bytesRecvd), fileSize);
			}
			/* Check if ACK from server is equal to the size of the file sent */
			if (ntohll(bytesRecvd) == fileSize) {
				printf("Server received the file successfully\n");
			} else {
				print_error("transfer_file: Server received the file unsuccessfully", 0, 0);
			}
		}
	}
}
