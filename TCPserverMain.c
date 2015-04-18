/**	@file TCPserverMain.c
 * 	@brief Contains the main program for starting the TCP server.
 *	Calls all functions to create server socket and connections and beginning running
 *	the server. 
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */
 
#include "TCPserver.h"

/**	@brief 	The main program for running the TCP server.
*	@param 	no parameters. 
*	@return returns 0 to the OS when main completes. 
*/
int main(){

  int listensockfd;
  struct hostent *hostptr; 
  struct sockaddr_in servaddr;
  
  listensockfd = create_TCP_Socket();  //create the TCP socket 
  hostptr = info_Host(); //get information about the host 
  servaddr = destination_Address(hostptr); //get the server ip address 
  servaddr = bind_Socket(listensockfd, servaddr); //bind a socket for the server program 
  servaddr = listen_On_Socket(listensockfd, servaddr); //listens on a specific socket 
  print_Server_info(listensockfd, hostptr, servaddr); //print connection information 
  run_Server(listensockfd, servaddr); //run the server program and create detached pthreads for incoming client connections
  return 0;
}

