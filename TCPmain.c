/**	@file TCPmain.c
 * 	@brief Contains the main program for starting the TCP client in C.
 *	Calls all functions to create socket connections with the server and 
 *	send and receive messages. 
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */

 
#include "TCPclient.h"


/**	@brief 	Used for testing purposes to test the TCP Client in C. 
*	@param 	*message is the message to send to the server.
			*servDeset contains information about the destination server.
			sockfd is the socket that the server expects the message to be sent over. 
			*response is the response from the server. 
*	@return returns nothing. 
*/
void sendMessageTest( char * message, struct sockaddr_in *servDest, int sockfd, char *response );
 
 
/**	@brief 	The main program for running the TCP client.
*	@param 	no parameters. 
*	@return returns 0 to the OS when main completes. 
*/
int main(int argc, char**argv)
{
	if(argc == 3)
	{
		struct sockaddr_in servDest;
		int sockfd = -1;
		char response[MAX_MESSAGE];
		
		sockfd = createSocket(argv[1], atoi(argv[2]), (&servDest));   //create the TCP socket 
		if( sockfd != -1)
		{
			sendMessageTest("<echo>HelloWorld</echo>", &servDest, sockfd, response);
			sendMessageTest("<echo>HelloWorld</echo>\n", &servDest, sockfd, response);
			sendMessageTest("<echo>sfglk</echo>", &servDest, sockfd, response);
			sendMessageTest("", &servDest, sockfd, response);
			sendMessageTest("\n", &servDest, sockfd, response);
			sendMessageTest("<echo>New Line At End</echo>\n", &servDest, sockfd, response);
			sendMessageTest("<loadavg/>", &servDest, sockfd, response);
			sendMessageTest("<echo> Hello World <echo>", &servDest, sockfd, response);
			sendMessageTest("<echo></echo>", &servDest, sockfd, response);
			
			closeSocket(sockfd); //close the socket
		}
	}
	else
	{
		printf("Incorrect Number of Command Line Arguments\n");
		printf("./c_client <IP Address or Server Host Name> <Port Number>\n");
	}

	return 0;
}


/*
 **************************************************
 **************************************************
 */
void sendMessageTest( char * message, struct sockaddr_in *servDest, int sockfd, char *response ){ 
	if( sendRequest(sockfd, message, servDest) != -1) //send the server a string request 
		if( receiveResponse(sockfd, response) != -1) //the client receives a response from the server 
			printResponse(response); //print the response from the server
}

 

 