#include "decode.c"
#include "encode.c"
#include "stdbool.h"

#define LEN 2000


char * acknowledgement = "ACK";

/* Handles client messages */
void client_handler(int sockfd,struct sockaddr_in *rx_client){
	char rx_message[LEN]; /* Used to store client messages */
	char *ip_addr = inet_ntoa(rx_client->sin_addr); /* Extract client ip */
	int port_addr = rx_client->sin_port; /* Extract client port number */
	char tx_message[LEN+1]; /* Stores final message including Message_type */
	int s_read = 0;
	bool while_fl = true;
	/* This is an indefinite loop for handling client communication */
	while(while_fl) 
	{
		/* Clearing the buffers */
		bzero(rx_message, LEN*sizeof(char));
		bzero(tx_message, LEN*sizeof(char));
		
		/* message from client */
		s_read = read(sockfd,rx_message,LEN); 
		/* If no error returned by read */
		if(s_read>=0)
		{
			printf("\n");
			printf("Encoded message received from client ");						
			printf("%s:%d with Message Type - %c is :",ip_addr,port_addr,rx_message[0]);
			fflush(stdout); 
			
			/* Display Base64 coded sent by client */
			write(1,rx_message+1,s_read-1); 
			printf("\n");
			
			/* message contents, except byte 0 sent for decoding */
			char *decoded_message = decode(rx_message+1);
			printf("Decoded message received from client ");
			printf("%s:%d is: ",ip_addr,port_addr);
			fflush(stdout);

			/* Display decoded sent by client */
			write(1,decoded_message,strlen(decoded_message)); 
			printf("\n");
			
			/* Following if construct handles Message_Type 3 sent by client for closing connection */
			if(rx_message[0]=='3')
			{
				printf("Client %s:%d sent CLOSE CONNECTION \n ",ip_addr,port_addr);
				fflush(stdout);
				printf("Closing connection with client %s:%d \n ",ip_addr,port_addr);
				/* Flag for exiting while loop */
				while_fl = false;
			}
			else
			{
				/* Encoding the ACK message from server */
				char * ack = encode(acknowledgement);
				/* Adding Message_Type 2 to the encoded message */
				snprintf(tx_message,sizeof(tx_message),"%c%s",'2',ack); 
				/* Sending message to client */
				write(sockfd,tx_message,strlen(tx_message)); 
			}
		}
	}
	close(sockfd); // free socket resources
	printf("Connection Closed \n ");
	exit(0);
}

int main(int argc, char *argv[])
{

	struct sockaddr_in client, server; // storing server,client internet addresses
	socklen_t client_len;
	bool while_flag = true;;
	/* Handles wrong input syntax */
	if(argc != 2)
	{
		printf("Use syntax <executable> <port>, e.g. ./server 8080\n");
	}
	else
	{
		/* Handles wrong input port */
		if((atoi(argv[1]) < 0 || atoi(argv[1]) > 65535))
		{
			printf("Enter an unsigned integer between 0 and 65535 \n");
		}	
		else
		{
			/* socket() function call creates an endpoint for the communication and return a file descriptor
			 that refers to that endpoint . the file descriptor returned by the succesfull call will be the	
			 lowest number file not currently open for the process
			 It takes the 3 arguments:
			 1. DOMAIN : this selects the protocol family which will be used for communication. 
						These families are defined in <sys/socket.h>.The formats currently understood by the Linux kernel include:
						AF_INET-: IPv4 internet protocols
						AF_INET6-: IPv6 Internet protocols			 
			 2. TYPE : SOCK_STREAM for stream sockets and SOCK_DGRAM for datagram sockets
			 3. PROTOCOL : '0' indicates that the OS will choose the appropriate protocol			 
			 */
			int srv_socket = socket(AF_INET,SOCK_STREAM,0); 
			
			/* Handling stderr error */
			if(srv_socket==-1)
			{
				printf("Socket creation failure. Exiting... \n");
			} 
			else
			{
				printf("Socket created. \n");
				server.sin_family = AF_INET;  /* IPv4 */
				server.sin_addr.s_addr = htonl(INADDR_ANY); /* Accepting all machine ip */

				server.sin_port = htons(atoi(argv[1])); /* network byte order */
				bzero(&server.sin_zero,8); /* clearing sin_zero */
				socklen_t len = sizeof(struct sockaddr_in); // len is the length of the socket address.
				
				/* Binding socket to addresses */
				int bind_status = bind(srv_socket,(struct sockaddr *) &server, sizeof(struct sockaddr_in));
				/* Handling stderr error */
				if(bind_status == -1)
				{
					printf("Failed to bind address to socket. Exiting... \n");
				} 
				else
				{
					printf("Binding successful. \n");
					
					/* Listening for incoming connections. We shall use a backlog queue length of 10. */
					int listen_status = listen(srv_socket,10);
					if(listen_status == -1)
					{
						printf("Failed to listen to the socket. Exiting... \n");
					} 
					else
					{
						printf("Listening at port %d\n", atoi(argv[1]));
						/* Infinite loop for handling multiple clients */
						while(while_flag)
						{
							client_len = sizeof(client);
							/*  Retrieve a connect request and convert that into a request. Returns a new file descriptor */
							int client_fd = accept(srv_socket,(struct sockaddr *) &client,&client_len); 
											 
					 		/* Handling stderr error */
							if(client_fd==-1)
							{
								printf("Failed to accept. Exiting... \n");								
							}
							else
							{
								/* Handling multiple child processes */
								int fork_status=fork(); 
						 
						 		/* Sending file descriptor for handling client message */
								if(fork_status==0)
								{
									close(srv_socket);
									client_handler(client_fd,&client);
									while_flag = false;
								} 
								else if(fork_status==-1)
								{
									printf("FORKING ERROR\n");
									while_flag = false;
								} 
								else /* Client message handled. Closing file descriptor after sending ACK */
								{
									close(client_fd); 
								}
							}
							
						}
					}
					
				}
				
			}

		
		}
		
	}
	return 0;
}
