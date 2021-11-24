/*
 ============================================================================
 Name        : clientTcp.c
 Author      : Siciliano Bartolomeo
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <limits.h>  // for INT_MAX, INT_MIN
#include <ctype.h>
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock();

void errorhandler(char *error_message);

int argvToInt(char *argv[], int i);

void parseCommand(char* string);

int main(int argc, char *argv[]) {

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	char* serverIp = "";
	int serverPort = 0;
	//if user inserts ip_address or port-number on command line
	if (argc>1){
    		switch(argc){
    		//ip-address
    		case 2:
    			serverIp = argv[1];
    			serverPort = PROTOPORT;
    			break;
    			//ip-address + port-number
    		case 3:
    			serverIp = argv[1];
    			serverPort = argvToInt(argv, 2);
    		break;
    		default:
    			printf("\nToo many arguments in input!\n"
    					"The maximum number is 2 (ip-adress, port-number).\n"
    					"Using default ip-address port-number.\n");
    			serverIp = IP_ADDRESS;
    			serverPort = PROTOPORT;
    		break;
    	}
    		//no argument specified
	}else {
    	serverIp = IP_ADDRESS;
    	serverPort = PROTOPORT;
    }

	printf("======================================================================");
	printf("\n|                      CLIENT-SERVER CALCULATOR                      |");
	printf("\n======================================================================");
	printf("\n|The program implements a calculator via client-server communication.|"
           "\n|The program can be started via the command line with any additional |"
           "\n|parameters, specifying the ip-address and port-number of the server |"
           "\n|as follows:                                                         |"
		   "\n|clientTcp.exe xxx.xxx.xxx.xxx yyyyy                                 |");
	printf("\n|---------------------------------------------------------------------");
	printf("\n|Enter the commands as follows:                                      |"
			"\n|<operation> <number1> <number2>                                     |"
			"\n|                                                                    |"
			"\n|Where <operation> means:                                            |"
			"\n| + :  add;                                                          |"
			"\n| - :  sub;                                                          |"
			"\n| x :  multiplication;                                               |"
			"\n| / :  division;                                                     |"
			"\n| = :  close the program.                                            |"
			"\n|Numbers entered must be signed or unsigned integers.                |");
	printf("\n======================================================================\n");

	//creation socket
	int c_socket = 0;
	c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (c_socket < 0) {
	errorhandler("socket creation failed.\n");
	closesocket(c_socket);
	clearwinsock();
	return -1;
	}

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(serverIp); // IP del server
	sad.sin_port = htons(serverPort); // Server port

	// CONNECTION TO SERVER
	if (connect(c_socket, (struct sockaddr *)&sad, sizeof(sad))< 0)
	{
	errorhandler( "Failed to connect.\n" );
	closesocket(c_socket);
	clearwinsock();
	return -1;
	}
	printf("\nClient connects to server with success.");

	//read user operation
	while (strcmp(operation.operation, "=") != 0){

		char string[64] = "";
		printf("\n\nEnter the calculation---> ");
		fgets(string, 63, stdin);
		parseCommand(string);

	if(strcmp(operation.operation, "+") == 0 || strcmp(operation.operation, "x") == 0 ||
			strcmp(operation.operation, "-") == 0 || strcmp(operation.operation, "/") == 0 ||
			strcmp(operation.operation, "=") == 0){

	if (strcmp(operation.operation, "=") != 0){

		operation.term1 = htonl(operation.term1);
		operation.term2 = htonl(operation.term2);

	//client sends the operation to the server
	if (send(c_socket, operation.operation, strlen(operation.operation), 0) != strlen(operation.operation))
		 		 		{
		 		 		errorhandler("send() sent a different number of bytes than expected");
		 		 		closesocket(c_socket);
		 		 		clearwinsock();
		 		 		return -1;
		 		 		}

	if (send(c_socket, (void*) &operation.term1, sizeof(operation.term1), 0) != sizeof(operation.term1))
				 		 		{
				 		 		errorhandler("send() sent a different number of bytes than expected");
				 		 		closesocket(c_socket);
				 		 		clearwinsock();
				 		 		return -1;
				 		 		}

		if (send(c_socket, (void*) &operation.term2, sizeof(operation.term2), 0) != sizeof(operation.term2))
					 		 		{
					 		 		errorhandler("send() sent a different number of bytes than expected");
					 		 		closesocket(c_socket);
					 		 		clearwinsock();
					 		 		return -1;
					 		 		}

	strcpy(operation.result, " ");
	int read = recv(c_socket, operation.result, STRING_LENGHT-1, 0);
	    	if ( read<=0 )
	    					{
	    					errorhandler("recv() failed or connection closed prematurely");
	    					closesocket(c_socket);
	    					clearwinsock();
	    					return -1;
	    					}
	    	operation.result[read] = '\0';

	//case division with 0
	if (strstr(operation.result, "Error") != NULL) {
		printf("%s" ,operation.result);
	} else {
	printf("Result: %s" ,operation.result);
	}
		} else {
			// send '=' to server
			if (send(c_socket, operation.operation, strlen(operation.operation), 0) != strlen(operation.operation))
					 		 		{
					 		 		errorhandler("send() sent a different number of bytes than expected");
					 		 		closesocket(c_socket);
					 		 		clearwinsock();
					 		 		return -1;
					 		 		}
		}
	}else{
		printf("\nThe symbol entered is not valid!");
	}

	}

	// CLOSE CONNECTION
	closesocket(c_socket);
	clearwinsock();
	printf("\nProgram finished.");
	return 0;
}

void parseCommand(char* string){
	operation.operation[0] = string[0];
	strtok(string, " ");

	operation.term1 = atoi(strtok(NULL, " "));
	operation.term2 = atoi(strtok(NULL, " "));
}

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void errorhandler(char *error_message) {
printf("%s",error_message);
}

int argvToInt(char *argv[], int i){
	   char *p;
	    int num;

	    errno = 0;
	    long conv = strtol(argv[i], &p, 10);

	    // Check for errors: e.g., the string does not represent an integer
	    // or the integer is larger than int
	    if (errno != 0 || *p != '\0' || conv > INT_MAX || conv < INT_MIN) {
	        // Put here the handling of the error, like exiting the program with
	        // an error message
	    } else {
	        // No error
	        num = conv;
	        return num;
	    }
	    return -1;
}
