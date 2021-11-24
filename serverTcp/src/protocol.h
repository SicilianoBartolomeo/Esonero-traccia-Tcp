/*
 * Protocol for client-server communication.
 */

#define IP_ADDRESS "127.0.0.1" //default server ip-adress (loopback)

#define PROTOPORT 60000 // default port number

#define QLEN 5 // size of request queue

#define STRING_LENGHT 31

struct{
	char operation[STRING_LENGHT+1];
	int term1;
	int term2;
    char result[STRING_LENGHT+1];
}operation;

