/*
 ============================================================================
 Name        : serverTcp.c
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
#include <ctype.h>
#include "protocol.h"

void errorhandler(char *errorMessage);

void clearwinsock();

int add(float a, float b);

int sub(float a, float b);

int mult(float a, float b);

float division(float a, float b);

int main(int argc, char *argv[]) {

	printf("\nProgram started.");

	WSADATA wsaData;
	WORD wVersionRequested;
	wVersionRequested= MAKEWORD(2,2);
	int iResult = WSAStartup(wVersionRequested, &wsaData);
	if (iResult < 0) {
	printf("Error at WSAStartup()\n");
	printf("A usable WinSock DLL cannot be found");
	return -1;
	}
	// The WinSock lib is accessible. Proceed
	printf("\nNo errors occurred. \n");

	//CREATE SOCKET
	int my_socket = 0;
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (my_socket < 0) {
	errorhandler("socket creation failed.\n");
	clearwinsock();
	return -1;
	}

	// BIND
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	sad.sin_port = htons(PROTOPORT);
	if (bind(my_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
	errorhandler("\nbind() failed.\n");
	closesocket(my_socket);
	clearwinsock();
	return -1;
	}

	// LISTEN
	if (listen (my_socket, QLEN) < 0) {
	errorhandler("\nlisten() failed.\n");
	closesocket(my_socket);
	clearwinsock();
	return -1;
	}

	// ACCEPT NEW CONNECTION
	struct sockaddr_in cad; // structure for the client address
	int client_socket; // socket descriptor for the client
	int client_len; // the size of the client address
	printf("\nWaiting for a client to connect...");

	while (1) {
	client_len = sizeof(cad); // set the size of the client address

	if ((client_socket = accept(my_socket, (struct sockaddr
	*)&cad, &client_len)) < 0) {
	errorhandler("accept() failed.\n");
	closesocket(my_socket);
	clearwinsock();
	return 0;
	}

	getsockname(client_socket, (struct sockaddr *) &cad, &client_len);
	printf("\nConnection established with %s:%u", inet_ntoa(cad.sin_addr), ntohs(cad.sin_port));

	strcpy(operation.operation, "");
    while (strcmp(operation.operation, "=") != 0){

    	int read1 = recv(client_socket, operation.operation, STRING_LENGHT-1, 0);
    	if ( read1<=0 )
    					{
    					errorhandler("recv() failed or connection closed prematurely");
    					closesocket(client_socket);
    					clearwinsock();
    					}

    	if (strcmp(operation.operation, "=") != 0){
    	int read2 = recv(client_socket, (void*) &operation.term1, sizeof(operation.term1), 0);
    	int read3 = recv(client_socket, (void*) &operation.term2, sizeof(operation.term2), 0);
    	if ( read2<=0 ||read3<=0)
				{
				errorhandler("recv() failed or connection closed prematurely");
				closesocket(client_socket);
				clearwinsock();
				}

    	operation.operation[read1] = '\0';
    	operation.term1 = ntohl(operation.term1);
    	operation.term2 = ntohl(operation.term2);

    	if(strcmp(operation.operation, "+") == 0){
    	    		int result = add(operation.term1, operation.term2);
    	    		sprintf(operation.result, "%d", result);
    	    	}
    	    	else if(strcmp(operation.operation, "-") == 0){
    	    		int result = sub(operation.term1, operation.term2);
    	    		sprintf(operation.result, "%d", result);
    	    	}
    	    	else if(strcmp(operation.operation, "x") == 0){
    	    		int result = mult(operation.term1,operation.term2);
    	    		sprintf(operation.result, "%d", result);
    	    	}
    	    	else if((strcmp(operation.operation, "/") == 0) && (operation.term2 != 0)){
    	    		float result = division(operation.term1, operation.term2);
    	    		sprintf(operation.result, "%.2f", result);
    	    	}
    	    	else if((strcmp(operation.operation, "/") == 0) && (operation.term2 == 0)){
    	    		strcpy(operation.result, "\nError! Operation not valid.");
    	    	}

    	 if (send(client_socket, operation.result, strlen(operation.result), 0) != strlen(operation.result))
    	 			 		 		{
    	 			 		 		errorhandler("send() sent a different number of bytes than expected");
    	 			 		 		closesocket(client_socket);
    	 			 		 		clearwinsock();
    	 			 		 		}

    	}
	  }  // end-while
	}
	return 0;
}


int add(float a, float b){
	return a + b;
}

int sub(float a, float b){
	return a - b;
}

int mult(float a, float b){
	return a * b;
}

float division(float a, float b){
		return a / b;
}

void clearwinsock() {
#if defined WIN32
WSACleanup();
#endif
}

void errorhandler(char *errorMessage) {
printf ("%s", errorMessage);
}

