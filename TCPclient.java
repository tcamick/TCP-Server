/**	@file TCPclient.java
 * 	@brief Contains the function implementations of creating and running a TCP client in JAVA.
 *	It creates a socket for sending client connections.
 *	It is capable of sending and receiving request to the TCP Server.
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */
/*
 * TCPclient.java
 * Systems and Networks II
 * Project 1
 *
 * This file describes the functions to be implemented by the TCPclient class
 * You may also implement any auxillary functions you deem necessary.
 */
import java.net.*;
import java.io.*;
import java.util.Scanner;


public class TCPclient
{
	/*
	* Instance Fields 
	*/
	private Socket _socket; // the socket for communication with a server
    private OutputStream sendStream; //stream for sending message to the server 
    private InputStream recvStream; //stream for receiving message from the server
    
       
	/**
	 * Constructs a TCPclient object.
	 */
	public TCPclient()
	{
	}

	/**
	 * Creates a streaming socket and connects to a server.
	 *
	 * @param host - the ip or hostname of the server
	 * @param port - the port number of the server
	 *
	 * @return - 0 or a negative number describing an error code if the connection could not be established
	 */
	public int createSocket(String host, int port)
	{
        try{
        _socket = new Socket(host, port);
        sendStream = _socket.getOutputStream();
        recvStream = _socket.getInputStream();
        }
		catch(Exception ex){
        	System.out.println("Socket Stream Failed");
        	return -1;
        }
        return 0;
	}
    
	/**
	 * Sends a request for service to the server. Do not wait for a reply in this function. This will be
	 * an asynchronous call to the server.
	 *
	 * @request - the request to be sent
	 *
	 * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int sendRequest(String request)
	{
		if(request.length() == 0) {  //null message is attempting to be sent to the server - do not send it - return -1
			System.err.println("ERROR: Cannot Send NULL Message to the Server");
			return -1;		
        }
		try{
            byte[] sendBuff = new byte[request.length()];
            sendBuff = request.getBytes();
            sendStream.write(sendBuff, 0, sendBuff.length); //send the message to the server
        }
		catch(Exception ex){
            System.err.println("Exception in sendRequst");
        	return -1;
		}
		return 0;
	}

	/**
	 * Receives the server's response. Also displays the response in a
	 * clear and concise format.
	 *
	 * @return - the server's response or NULL if an error occured
	 */
	public String receiveResponse()
	{
		String response = "";		
		try{
            int dataSize;
            while((dataSize = recvStream.available()) == 0);
            byte[] recvBuff = new byte[dataSize];
            recvStream.read(recvBuff, 0,dataSize);
            response = new String(recvBuff,0,dataSize); //receive the message from the server
        }
		catch (Exception ex){
            System.err.println("Exception in receiveResponse");
        	return null;
		}
		return response;
	}
	
	/*
     * Prints the response to the screen in a formatted way.
     *
     * response - the server's response as an XML formatted string
     *
     */
	public static void printResponse(String response)
	{
		System.out.println("Response from server: " + response);  
	}
    
    
	/*
	 * Closes an open socket.
	 *
     * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int closeSocket()
	{
		try{
		    sendStream.close();  
		    recvStream.close();    	
		    _socket.close();    
		}
		catch(Exception ex){  
		    System.err.println("Exception in close"); 
			return -1;
		}
		return 0;
	}
    
	
	/**
	 * The main function. Use this function for
	 * testing your code. We will use our own main function for testing.
	 */
	public static void main(String[] args)
	{
		String response = "";
		if (args.length == 2){
		
			final String servName = args[0];
		    final int servPort = Integer.parseInt(args[1]);	
		    
			TCPclient client = new TCPclient();  
		    if ( client.createSocket(servName, servPort)  >= 0 ) //create the TCP socket 
		    {
				sendMessageTest("<echo>sfglk</echo>", response, client);
				sendMessageTest("", response, client);
				sendMessageTest("\n", response, client);
				sendMessageTest("<echo>sf\nglk</echo>", response, client);
				sendMessageTest("<echo>New Line At End</echo>\n", response, client);
				sendMessageTest("<loadavg/>", response, client);
				sendMessageTest("<echo>Hello World<echo>", response, client);
				sendMessageTest("<echo></echo>", response, client);
			
				client.closeSocket();    //close the TCP socket 
			}
		}
		else {
			System.out.println("Incorrect Number of Command Line Arguments");
			System.out.println("java TCPclient <IP Address or Server Host Name> <Port Number>");
		}
	}
	
	
	/**
	 * Used for testing the main program by sending request and receiving 
	 * responses from the server. 
	 */
	public static void sendMessageTest(String message, String response, TCPclient client) 
	{
		if( client.sendRequest(message) >= 0 ) //send a request to the server 
		{    
			response = client.receiveResponse(); //receive a response from the server
			client.printResponse(response); //print the response
		}
	}
	
}



