#include "encode.c"
#include "decode.c"

#define MAX_LEN 3000
#define true 1

void initialise_buff(char *msg,char *buff); //function to initialise buffers

int main(int argc, char *argv[])
{

	struct sockaddr_in server; // structure containing an internet address.
	
	char *user_inp = "\n\nEnter 'y' to send message or 'n' to close connection: " ;
	char buff_send[MAX_LEN+1];   
	char entered_choice[2]; // stores char entered by user	
	char server_msg[MAX_LEN]; 

	int sid = socket(AF_INET,SOCK_STREAM,0); 
	
	// prints "SOCKET FAILURE" when socket system call fails 
	if(sid==-1){
		perror("SOCKET FAILURE");
		exit(-1);
	} 

	// receives IP address of server from the terminal and convert it into network address
	if(inet_aton(argv[1],&server.sin_addr)==0)
	{
		write(1,"Server Address Error",strlen("Server Address Error"));
	} 

	int port_no = atoi(argv[2]); // extract port number 
	server.sin_family = AF_INET; // for IPv4 protocol
	server.sin_port = htons(port_no); // converts port number integer to network bytes order  
	socklen_t len = sizeof(struct sockaddr_in);  //length of socket address.

	// Establish connection with the server. Function takes three arguments, the socket file descriptor, the address of the host to 
	// which it wants to connect and length of the address.
 
	if(connect(sid,(struct sockaddr *) &server, len)==-1)
	{
		perror("SOCKET CONNECTION ERROR");
		exit(-1);
	} 

	write(1,"\nConnection is established : ",strlen("\nConnection is established : ")); 

	// Infinite loop for sending messages to server
	while(true)
	{		
		initialise_buff(server_msg,buff_send); // buffers initialisation 
		write(1,"\nEnter message to send to server : ",strlen("\nEnter message to send to server : ")); 		
		int i=0;
		while(true)
		{ 
			read(0,entered_choice,1); // read characters entered through terminal             
			if((entered_choice[0]=='\n')||(i==(MAX_LEN-1))) // stop reading 
				break;
			server_msg[i] = entered_choice[0];  
			i++;
		}

		char *encoded_msg= encode(server_msg); // encoding the message to Base64
		snprintf(buff_send,sizeof(buff_send),"%c%s",'1',encoded_msg); // appending message with message type
		write(sid,buff_send,strlen(buff_send)); // writing complete message to socket
		
		initialise_buff(server_msg,buff_send);
		read(sid,server_msg,40); // read message sent by server
		strcpy(server_msg+1,decode(server_msg+1)); // decoding the received message

		if(server_msg[0]!='2')
		{ 
			write(1,"Acknowledgement not received",strlen("Acknowledgement not received")); 
			continue; 
		}
		else
		{
			write(1,"Message received from server : ",strlen("Message received from server : "));			
			fflush(stdout); // Flush the buffer. 
			write(1,server_msg+1,strlen(server_msg)-1); // print message from server
			
		}

		write(1,"\n\nEnter 'y' to send message or 'n' to close connection: ",strlen("\n\nEnter 'y' to send message or 'n' to close connection: ")); 	
		
		read(0,entered_choice,2);		

		if((entered_choice[0]=='n')||(entered_choice[0]=='N')) // command for closing connection
		{ 
			char * encoded_msg = encode("close"); // Encoding the connection closing message into base64
			snprintf(buff_send,sizeof(buff_send),"%c%s",'3',encoded_msg); // appending message with message type
			write(sid,buff_send,strlen(buff_send)); // writing close message to socket
			write(1,"Connection Closed by user\n",strlen("Connection Closed by user\n")); 
			break;
		}
	}


	close(sid); // close and release socket resources

return 0;
}

// function for initialising buffers holding client and server messages
void initialise_buff(char *msg,char *buff)
{
	int index=0;
	for(index=0; index<MAX_LEN; index++)
	{
		msg[index] = '\0';
		buff[index] = '\0';
	}
	buff[index]='\0';
}

