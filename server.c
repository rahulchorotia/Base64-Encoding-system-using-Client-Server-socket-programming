#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>    		//close  
#include <arpa/inet.h>    //close  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#include <string.h>
#include <stdlib.h>	
#define MSG_LEN 256
typedef int ll;

struct mail{
	ll message_type;
	char message[MSG_LEN];   
};


void string_to_bin(char * input,ll *bin,ll n){ //Function to get binary representation of encoded string
	for(ll i=n;i<n+4;i++){
		ll temp;
		
		if(input[i]>='A' && input[i]<='Z'){  
		 // First get their index value in the index set used in encoder
			int x = input[i]-'A';
			temp=x;
		}
		
		else if(input[i]>='a' && input[i]<='z'){
			int x1 = input[i]-'a'+26;
			temp=x1;	
		}
		
		else if(input[i]>='0' && input[i]<='9'){
			int x2 =input[i]-'0'+52;
			temp=x2;
		}
		
		else if(input[i]=='+'){
			temp=62;
		}
		
		else if(input[i]=='/'){
			temp=63;
		}
		
		else{
			temp=0;
		}
		
		for(ll j=5;j>=0;j=j-1){ // Convert to binary
			bin[j+(i-n)*6]=temp%2;
			temp=temp/2;
		}
	}
}


void convert_to_string(char *output,ll* bin,ll s,ll type){ // Taking a group of 24 bits and converting into 3 characters
	for(ll i=0;i<3;i++){
		ll temp=0;
		ll val=1;
		
		for(ll j=7;j>=0;j--){
			temp+=val*bin[j+i*8];
			val=val*2;
		}
		
		temp=temp-97;
		output[s+i]='a'+temp;
	}
}


void decode(char *input,ll n,char *output){
	ll bin[24];
	if(n%4){
		printf("Wrong input\n");
		return;
	}

	ll temp=(3*n)/4;
	if(input[n-2]=='='){	// When input(un-encoded) is of form 3n+1
		temp=temp-2;
	}

	else if(input[n-1]=='='){ // When input(un-encoded) is of form 3n+2
		temp=temp-1;
	}
	
	else{
		temp=3*n/4;
	}

	for(ll i=0;i<n;i+=4){	// Converting group of 24 bits to binary then string
		string_to_bin(input,bin,i);
		convert_to_string(output,bin,3*i/4,0);
	}

	output[temp]='\0';
}




void error(char *msg){
		perror(msg);
		exit(1);
}

ll main(ll argc, char *argv[]){
	ll max_sd ;
	ll max_clients ;
	max_clients = 30 ;  
	ll client_socket[max_clients] ;
	ll probing; 
	ll i ;
	ll sd;    
	ll sockfd;
	ll newsockfd;
	ll portno;
	ll clilen;
	char* buffer;
	buffer = (char *)malloc(MSG_LEN*sizeof(char)); // Server buffer
	struct sockaddr_in serv_addr, cli_addr;
	ll n=0;
	fd_set readfds; 	// List of socket fds

	for (i = 0; i <= max_clients-1; i=i+1){     // client_socket is a bitmap denoting occupied/non-occupied sockets
		client_socket[i] = 0;   
	}

	if (argc < 2){
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);	// Socket alloted
	
	if (sockfd <= -1) {
		error("ERROR opening socket");
	}
	ll input =sizeof(serv_addr);
	bzero((char *) &serv_addr, input); // Fields of serv_addr filled before binding
	char *input1=argv[1];
	portno = atoi(input1);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}
	ll five =5;
	listen(sockfd,five); // Actively listening for connection from client
	struct mail msg;
	ll true =1;
	while(true){
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		max_sd = sockfd;
		
		for ( i = 0 ; i <= max_clients-1 ; i++){
			  
			sd = client_socket[i];   
			  

			if(sd >= 1){   	// Only sd which are in use/ value =1 are added
				FD_SET( sd , &readfds);
			}   
			  

			if(sd >= max_sd+1){  // Max_sd needed for select later
				max_sd = sd;
			}   
		}
			 
		probing = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  // Select gets activated if any sockfd in readfs is active

		if ((probing <= -1)){   
			error("select error");   
		}   	
					 
		if (FD_ISSET(sockfd, &readfds)){ // Checks if there is activity on main socket, indicating new connection
			clilen = sizeof(cli_addr);
			newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // Accept the incoming connection

			if (newsockfd <= -1){
				error("ERROR on accept");
			}
			
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , newsockfd , inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port)); 
			n = write(newsockfd,"Welcome to team",15);
			if (n <= -1){
				error("ERROR writing to socket");
			}
					
			//puts("Welcome message sent successfully\n");   
			
			for (i = 0; i <= max_clients-1; i++){
				// Add new connection into forst non-zero entry of client_socket  
				if( client_socket[i] == 0 ){
					client_socket[i] = newsockfd;   
					printf("Adding to list of sockets as %d\n" , i);   
					break;   
				}   
			}   
		}
		 

		// Else its some IO operation on some other socket 

		for (i = 0; i <= max_clients-1; i++){
			sd = client_socket[i];  
			if (FD_ISSET( sd , &readfds)){ // Check if that particular sd has activity
				bzero(buffer,MSG_LEN);
				n = read(sd,buffer,MSG_LEN);
				
				if (n <= -1){
					error("ERROR reading from socket");
				}

				//Closing case
				if (!n){   
                     			getpeername(sd , (struct sockaddr*)&cli_addr ,(socklen_t*)&cli_addr);
                    			printf("Host disconnected , ip %s , port %d\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));            
                    			close( sd );   // Close socket
                    			client_socket[i] = 0;   // Clear bitmap
                		}    
     			

				msg.message_type=buffer[0]-'0'; // Get message type from first byte
				strcpy(msg.message,buffer+1);
			
				if(msg.message_type==1){
					printf("Here is the message : %s\n",msg.message);
					char output[3*strlen(msg.message)/4];
					decode(msg.message,strlen(msg.message),output); // Decode the base64 message
					printf("Decoded output: %s\n",output);
					bzero(buffer,MSG_LEN);
					buffer[0]='2';
					strcpy(buffer+1,"Here is the Acknowledgement for your previous message");
					n = write(sd,buffer,MSG_LEN); // Send back an ACK
					
					if (n < 0){ 
						error("ERROR writing to socket");
					}

				}
				
				else if(msg.message_type==3){ // 3 type is for connection closing
					getpeername(sd , (struct sockaddr*)&cli_addr ,(socklen_t*)&cli_addr);
					printf("Host disconnected , ip %s , port %d \n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));                        
					close( sd );   // Close socket
					client_socket[i] = 0;   // Clear bitmap
				}
			}    
		}
	}    
	free(buffer); // Deallocate server buffer
	return 0; 
}
