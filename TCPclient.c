/**	@file TCPclient.c
 * 	@brief Contains the function implementations of creating and running a TCP client in C. 
 *	It creates a socket for sending client connections. 
 *	It is capable of sending and receiving request to and from the TCP Server. 
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */

#include "TCPclient.h" 


/*
 **************************************************
 *		FUNCTION PROTOTYPES
 **************************************************
 */
 
/**	@brief 	Is a function that takes in a error message and outputs the message to the display
*			and stops the server from running. 
*	@param 	Is the error message that should be outputted to the screen.
*	@return returns a -1 integer.
*/
int printErrorMessage( char *message );

/**	@brief Create a TCP socket and return the integer that represents the socket. 
*	@return the socket number. 
*/
int create_TCP_Socket(void);

/**	@brief 	get IP address of the server that the client will send messages to. 
*	@param 	serverName is the name of the server. 
*	@return returns a structure hostent that contains IP information about server. 
*/
struct hostent *info_Host(char * serverName);

/**	@brief Set the destination address and port of the server. 
*	@param 	hostptr is the structure containing information about the server 
*			port is the port number to connect to the server 
*			dest contains the destination IP address information. 
*	@return returns nothing.
*/
void setDestination(struct hostent *hostptr, int port, struct sockaddr_in *dest);

/*
 **************************************************
 *		CLIENT FUNCTIONS
 **************************************************
 */

/*
 **************************************************
 **************************************************
 */
int printErrorMessage( char *message ) {	
  fprintf(stderr, "ERROR: %s\n", message);
  return -1;
}

/*
 * Creates a streaming socket and connects to a server.
 *
 * serverName - the ip address or hostname of the server given as a string
 * port       - the port number of the server
 * dest       - the server's address information; the structure should be created with information
 *              on the server (like port, address, and family) in this function call
 *
 * return value - the socket identifier or a negative number indicating the error if a connection could not be established
 */
int createSocket(char * serverName, int port, struct sockaddr_in * dest){
	int listensockfd = 0;
	struct hostent *hostptr; 

	//create the socket
	listensockfd = create_TCP_Socket();
	if(listensockfd == -1) return -1;

	//get client host information
	hostptr = info_Host(serverName);
	if(hostptr == NULL) return -1;
	
	//set server destination
	setDestination(hostptr, port, dest);
	
	//connect to the server
	if(connect(listensockfd, (struct sockaddr *) dest, sizeof(*dest)) == -1)
		return printErrorMessage("Cannot Connect to the Server");
	
	//return the listening socket
	return listensockfd;
}


/*
 **************************************************
 **************************************************
 */
int create_TCP_Socket(void){
	int listensockfd = socket(AF_INET, SOCK_STREAM,0);
	if(listensockfd == -1)
		return printErrorMessage("Cannot Open Socket to Listen"); 
	return listensockfd;
}

/*
 **************************************************
 **************************************************
 */
struct hostent *info_Host(char * serverName){
	struct hostent *hostptr; 
	hostptr = gethostbyname(serverName);
	if(hostptr == NULL) 
		printErrorMessage("That Host Does Not Exist");			
	return hostptr;
}

/*
 **************************************************
 **************************************************
 */
void setDestination(struct hostent *hostptr, int port, struct sockaddr_in *dest){
	memset((void *) dest, 0, (size_t)sizeof(struct sockaddr_in));  
	memcpy((void *) &(dest->sin_addr), (void *) hostptr->h_addr, hostptr->h_length);
	dest->sin_family = (AF_INET);
	dest->sin_port = htons((u_short) port);
}


/*
 * Sends a request for service to the server. This is an asynchronous call to the server, 
 * so do not wait for a reply in this function.
 * 
 * sock    - the socket identifier
 * request - the request to be sent encoded as a string
 * dest    - the server's address information
 *
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int sendRequest(int sock, char * request, struct sockaddr_in * dest){
	int error = 0;
	if(request[0] == '\0') {
		printErrorMessage("Cannot Send NULL Message to the Server");
		return -1;
	}
	error = sendto(sock, request, strlen(request), 0, (struct sockaddr *) dest, sizeof(*dest));
	if(error == -1) return -1;
	return 0;
}

/*
 * Receives the server's response formatted as an XML text string.
 *
 * sock     - the socket identifier
 * response - the server's response as an XML formatted string to be filled in by this function;
 *            memory is allocated for storing response
 *
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int receiveResponse(int sock, char * response){
	int error = 0;
	bzero(response, MAX_MESSAGE);
	error = recvfrom(sock, response, MAX_MESSAGE, 0, NULL, NULL);
	if(error == -1) return -1;
	return 0;
}

/*
 * Prints the response to the screen in a formatted way.
 *
 * response - the server's response as an XML formatted string
 *
 */
void printResponse(char* response){
	printf("Response from server: %s\n", response); 
}

/*
 * Closes the specified socket
 *
 * sock - the ID of the socket to be closed
 * 
 * return - 0, if no error; otherwise, a negative number indicating the error
 */
int closeSocket(int sock){
	int error = close(sock);
	if(error == -1) return -1;
	return 0;
}

