/**	@file TCPserver.c
 * 	@brief Contains the function implementations of creating and running a TCP server.
 *	It creates a socket, binds the socket and listen on that specific socket for incoming client connections.
 *	It automatically determines the local host information that the server is running on.
 *	Dynamically allocates a port to connect to as well. 
 *	This information can be outputted to the screen to allow clients to connect. 
 *	Messages can be sent to the server in the following format:
 *	<echo>message</echo>
 *	<loadavg/>
 *	If a message is sent that is not in the above format, 
 *	server responses with <error>unknown format</error>.
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */
 
#include "TCPserver.h"

/*
 **************************************************
 *		STRUCTURES
 **************************************************
 */

/*
 *	Used to store connected client information
 */
typedef struct ClientStruct{
  int confd;
  char message[MAX_MESSAGE];
  struct sockaddr_in clientaddr;
}ClientStruct_T, *ClientStruct_P;


/*
 **************************************************
 *		FUNCTION PROTOTYPES
 **************************************************
 */
 
/**	@brief 	Is a function that takes in a error message and outputs the message to the display
*			and stops the server from running. 
*	@param 	Is the error message that should be outputted to the screen.
*	@return returns nothing. 
*/
void printErrorMessage( char *message );

 
/**	@brief 	Is the function for create detached threads to handle the message sent from the client. 
*	@param 	is a void pointer to the client data structure and is typed cast internally back
*			to the correct structure type. 
*	@return returns a void pointer. 
*/
void *receiveMessage( void * param );


/**	@brief 	Modifies the sent message and return the modified message to the client. 
*	@param 	clientaddr is a structure containing the connected client identification and the
*			message that was sent to the server to be processed. 
*			recvMesg is a char array containing the client message that was sent to the server. 
*	@return returns nothing. 
*/
void handleMessage(ClientStruct_P clientaddr, char *recvMesg);


/**	@brief 	Determines if the message is a valid ECHO or LOADAVG command or
*			if the message is a error message, and makes decisions based upon this.
*	@param 	*recvMesg is a char array containing the client message that was sent to the server.
*			*send is the char array representing the message to be sent back to the client. 
*	@return returns nothing. 
*/
void modifyMessage(char *recvMesg, char *send);


/**	@brief 	The client sent a message in the ECHO header and should be returned to the client
*			in REPLY headers. 
*	@param 	*recvMesg is a char array containing the client message that was sent to the server. 
*			*send is the char array representing the message to be sent back to the client. 
*	@return returns nothing. 
*/
void echoMessage(char *recvMesg, char *send);


/**	@brief 	The client sent the <loadavg/> message and therefore the load average
*			on the server for 1:5:15 minutes.
*	@param 	*recvMesg is a char array containing the keyword -> <loadavg/>
*			*send is a char array containing the load average calculations. 
*	@return returns nothing. 
*/
void loadavgMessage(char *recvMesg, char *send);


/**	@brief	The client sent the server a invalid message and must be returned
*			to the client as a invalid input. 
*	@param 	*recvMesg is a char array contains the message that the client sent to the server.
*			*send is the char array representing the message to be sent back to the client. 
*	@return	returns nothing. 
*/
void errorMessage(char *recvMesg, char *send);


/**	@brief	Used to reverse a string pass into the function. 
*	@param	*original is the string to reverse, it directly modifies this char array 
*	@return	returns nothing. 
*/
void reverseString(char *original);


/*
 **************************************************
 *		SERVER FUNCTIONS
 **************************************************
 */
 
/*
 **************************************************
 **************************************************
 */
void printErrorMessage( char *message ) {	
  fprintf(stderr, "ERROR: %s\n", message);
  exit(1);
}


/*
 **************************************************
 **************************************************
 */
int create_TCP_Socket(void){
  int listensockfd = socket(AF_INET, SOCK_STREAM,0);
  if(listensockfd == -1)
	printErrorMessage("Cannot Open Socket to Listen"); 
  return listensockfd;
}


/*
 **************************************************
 **************************************************
 */
struct hostent *info_Host(void) {
  char hostname[IP_4];
  struct hostent *hostptr; 
  if(gethostname(hostname, IP_4) == -1) 
	printErrorMessage("Cannot Get The Host Name");
  hostptr = gethostbyname(hostname);
  if(hostptr == NULL) 
	printErrorMessage("That Host Does Not Exist");
  return hostptr;
}


/*
 **************************************************
 **************************************************
 */
struct sockaddr_in destination_Address( struct hostent *hostptr) {
  struct sockaddr_in servaddr;
  memset((void *) &servaddr, 0, (size_t) sizeof(servaddr));
  memcpy((void *) &servaddr.sin_addr, (void *) hostptr->h_addr, hostptr->h_length);
  servaddr.sin_family = (AF_INET);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons((u_short) 0);
  return servaddr;
}


/*
 **************************************************
 **************************************************
 */
struct sockaddr_in bind_Socket(int listensockfd, struct sockaddr_in servaddr){
  if(bind(listensockfd, (struct sockaddr *) &servaddr, (socklen_t) sizeof(servaddr)) == -1)
	printErrorMessage("Failed to Bind To Socket");  
  return servaddr;
}


/*
 **************************************************
 **************************************************
 */
struct sockaddr_in listen_On_Socket(int listensockfd, struct sockaddr_in servaddr){
  int servaddrlen = sizeof(struct sockaddr_in);
  if(getsockname(listensockfd, (struct sockaddr *) &servaddr, (socklen_t *) &servaddrlen) == -1)
	printErrorMessage("Cannot Listen To This Port");
  if(listen(listensockfd, MAX_NUM_LISTENER_ALLOWED) == -1)
	printErrorMessage("MAX Number of Connections Established");
  return servaddr;
}


/*
 **************************************************
 **************************************************
 */
void print_Server_info(int listensockfd, struct hostent *hostptr, struct sockaddr_in servaddr){
  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, INTERFACE, IFNAMSIZ-1);
  if(ioctl(listensockfd, SIOCGIFADDR, &ifr) == -1)
	printErrorMessage("Cannot Get Server IP Address");
  
  printf("\nHostname Name : %s\n", hostptr->h_name);
  printf("Host IP Address : %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  printf("Host Port Number : %i\n\n", htons(servaddr.sin_port));
}


/*
 **************************************************
 **************************************************
 */
void run_Server(int listensockfd, struct sockaddr_in servaddr){
  printf("Waiting for Clients ......\n\n");
  struct sockaddr_in cliaddr;
  socklen_t clilen = sizeof(cliaddr); 
  while(1)
  {
	int connfd = accept(listensockfd,(struct sockaddr *)&cliaddr,&clilen);
	if(connfd == -1)
		printErrorMessage("Cannot Accept the Incoming Connections"); 
	
	//thread id
	int pthread_error = 0;
	pthread_t tid;
	ClientStruct_T clientStruct_t;
	ClientStruct_P clientStruct_p = &clientStruct_t;
	clientStruct_p->confd = connfd;
	clientStruct_p->clientaddr = cliaddr;
	pthread_error = pthread_create(&tid,NULL, receiveMessage, (void *) clientStruct_p);
	if(pthread_error == 0) pthread_detach(tid);
  }
}


/*
 **************************************************
 **************************************************
 */
void *receiveMessage( void * param){
  ClientStruct_P clientStruct_p = (ClientStruct_P) param;
  char recvMesg[MAX_MESSAGE];
  int byteReceivedCount = 1;
 
  //continue receiving from the currently connected client 
  while(byteReceivedCount > 0) 
  {
	memset((void *) &recvMesg, 0, (size_t) sizeof(recvMesg));
	socklen_t clilen = sizeof(clientStruct_p->clientaddr);

  	//receive message from client 
  	byteReceivedCount = recvfrom(clientStruct_p->confd, recvMesg, MAX_MESSAGE, 0,(struct sockaddr *) &clientStruct_p->clientaddr, &clilen);

 	//only if client sent a message does it need processing
	if(byteReceivedCount > 0)
 		handleMessage(clientStruct_p, recvMesg);
   }
  close(clientStruct_p->confd); 
  pthread_exit(0);
}


/*
 **************************************************
 **************************************************
 */
void handleMessage(ClientStruct_P clientStruct_p, char *recvMesg){
  int byteSentCount = 0; 
  char sendMesg[MAX_MESSAGE];
  socklen_t clilen = sizeof(clientStruct_p->clientaddr);
  memset((void *) &sendMesg, 0, (size_t) sizeof(sendMesg));
  
  //remove newline character at ending if present
  if(recvMesg[ ( strlen(recvMesg) - NEW_LINE ) ]  == '\n') recvMesg[ ( strlen(recvMesg) - NEW_LINE ) ] = '\0';
  
  //print client message
  printf("***************************************************\n");
  printf("Received the following message from : %s\n%s\n", inet_ntoa(((struct sockaddr_in *) &(clientStruct_p->clientaddr))->sin_addr), recvMesg);
  
  //modify the incoming message 
  modifyMessage(recvMesg, sendMesg);
 
  //send the client the modified message
  byteSentCount = sendto(clientStruct_p->confd, sendMesg, strlen(sendMesg), 0, (struct sockaddr *) &clientStruct_p->clientaddr, clilen);
  
  printf("Sent the following message to : %s\n%s", inet_ntoa(((struct sockaddr_in *) &(clientStruct_p->clientaddr))->sin_addr), sendMesg);
  printf("\n***************************************************\n\n");
}


/*
 **************************************************
 **************************************************
 */
void modifyMessage(char *recvMesg, char *send){
  char *sendMesg = send;
  //handle <echo> messages
  if(!strncmp(recvMesg, "<echo>", ECHO_XML_START))
	echoMessage(recvMesg, sendMesg); 
  //handle <loadavg/> messages
  else if(!strncmp(recvMesg, "<loadavg/>", LOADAVG_XML))
   	loadavgMessage(recvMesg, sendMesg); 
  //handle error messages
  else	
	errorMessage(recvMesg, sendMesg); 
}


/*
 **************************************************
 **************************************************
 */
void echoMessage(char *recvMesg, char *send){
  char *sendMesg = send, reverseMesg[MAX_MESSAGE], modifiedReceiveMessage[MAX_MESSAGE];
  memset((void *) &reverseMesg, 0, (size_t) sizeof(reverseMesg));
  memset((void *) &modifiedReceiveMessage, 0, (size_t) sizeof(modifiedReceiveMessage));

  strcpy(reverseMesg, recvMesg);
  reverseString(reverseMesg); //reverse the string for the final 
  if((!strncmp(recvMesg, "<echo>", ECHO_XML_START)) && (!strncmp(reverseMesg, ">ohce/<", ECHO_XML_END))) //compare beginning and ending ECHO
  {
	strcpy(sendMesg, "<reply>");
	strncpy(modifiedReceiveMessage, recvMesg + ECHO_XML_START, strlen(recvMesg) - (ECHO_XML_START + ECHO_XML_END));
	strcat(sendMesg, modifiedReceiveMessage);
	strcat(sendMesg, "</reply>");
  }
  else	
	errorMessage(recvMesg, sendMesg); 
}


/*
 **************************************************
 **************************************************
 */
void loadavgMessage(char *recvMesg, char *send){
  char *sendMesg = send, time[MAX_MESSAGE];
  double loadAvg[LOAD_AVG_FUNCTION] = {0.0, 0.0, 0.0}; 
  getloadavg(loadAvg, LOAD_AVG_FUNCTION);
  strcpy(sendMesg, "<replyLoadAvg>");
  sprintf(time, "%f", loadAvg[LOAD_AVG_1_MIN_INDEX]);
  strcat(sendMesg, time);
  strcat(sendMesg, ":");
  sprintf(time, "%f", loadAvg[LOAD_AVG_5_MIN_INDEX]);
  strcat(sendMesg, time);
  strcat(sendMesg, ":");
  sprintf(time, "%f", loadAvg[LOAD_AVG_15_MIN_INDEX]);
  strcat(sendMesg, time);
  strcat(sendMesg, "</replyLoadAvg>");
}


/*
 **************************************************
 **************************************************
 */
void errorMessage(char *recvMesg, char *send){
  char *sendMesg = send;
  strcpy(sendMesg, "<error>unknown format</error>");
}


/*
 **************************************************
 **************************************************
 */
void reverseString(char *original){
  char temp;
  int i = 0, j = strlen(original) - NEW_LINE;
  for(i = 0; i < j; i++, j--)
  {	
	temp = original[i];
	original[i] = original[j];
	original[j] = temp;
  }
}



