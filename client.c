#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <unistd.h>            
#include <arpa/inet.h>
#include <string.h>  
#define MSG_LEN 256

char* 	ind="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";   //Index set used to get base64 character from value

struct mail{                      // Structure defined for the message
	int message_type;
	char message[MSG_LEN];
};

int power(int a,int b){           // Helper power function used in encoding
	int temp=1;

	while(1){
		if(b-- <=0)
			break;
		temp*=a;
	}
	return temp;
}

char * base(char *input,char *encode){	
	int n = strlen(input);
	int i;
	int k;
	int m=0;
	int l;
	int c;
	int temp;
	int j=0;
	int ans[24];
	int val;
	int groups = n/3;             // Total number of groups of 24 bits are groups+1

	for(k=0;k<=groups-1;k++)
	{          // All but last group are taken care of in this for loop
		for(i=0;i<=2;i++)
		{       // Take 3 characters at a time = 24 bits
			c=input[3*k+i];
			j=0;
			while(j<8){            // Binary representation calculated
				temp=c%2;
				ans[8*i+7-j]=temp;
				c=c/2;
				j++;
			}
		}
		j=0;
		for(l=0;l<=3;l++){          // 24 bits are grouped in 4 groups of 6 bits
			val=0;
			for(j=0;j<=5;j++){
				val+=ans[6*l+5-j]*power(2,j);
			}
			encode[m] = ind[val];      // Value of 6 bits is used to retrive characted from index set
			m = m+1;		
		}
	}

	int endcase1[12];
	for(int l =0;l<12;l++){
		endcase1[l]=0;
	}
	int endcase2[18];
	for(int l =0;l<18;l++){
		endcase2[l]=0;
	}
	int true =1;
	if(n%3 == true)
	{	                                // When number of characters are 3n+1
		c=input[3*groups];
		j=0;
		while(j<8){				// Binary representation of 1 character found
			temp=c%2;			// Padded with 4 zeros
			endcase1[7-j]=temp;
			c=c/2;
			j = j+1;
		}
		j=0;
		for(l=0;l<=1;l++){
			val=0;
			for(j=0;j<=5;j++){
				val=val+endcase1[6*l+5-j]*power(2,j);
			}
			encode[m] = ind[val]; 		// Similarly 2 groups of 6 bits are used to get 2 encoded characters
			m =m+1;		
		}
		encode[m]='=';
		m =m+1;	
		encode[m]='='; // Remaining 12 bits in the last group are filled with 2 =
		m =m+1;	
	}

	else if(n%3==2){                            // 3n+2 case, 2 characters = 16 bits + 2 padded zeros
		for(i=0;i<=1;i++){
			c=input[3*groups+i];
			j=0;
			while(j<8){
				temp=c%2;
				endcase2[8*i+7-j]=temp;
				c=c/2;
				j = j+1;
			}
		}
		j=0;
		for(l=0;l<=2;l++){
			val=0;
			for(j=0;j<=5;j++){
				val = val + endcase2[6*l+5-j]*power(2,j);
			}
			
			encode[m] = ind[val];
			m =m+1;			
		}	
		encode[m]='=';      // Last 6bits are compensated by 1 =
		m =m+1;	
	}
	encode[m] = '\0';
}



void error(char *msg){ // Helper error function
	perror(msg);
	exit(0);
}


int main(int argc, char *argv[]){
	int sockfd=0, portno=0, n=0;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char* buffer=(char *)malloc(MSG_LEN*sizeof(char));   // Allocating buffer space for socket

	if (argc <= 2) {
		char *x = argv[0];
		fprintf(stderr,"usage %s hostname port\n", x);
		exit(0);
	}
	char *x1 = argv[2];
	portno = atoi(x1);
	int zero =0;
	sockfd = socket(AF_INET, SOCK_STREAM, zero); //Create socket for client
	
	if (sockfd < 0){ 
		if(sockfd >=0)
			exit(0);
		error("ERROR opening socket");
	}
	char *x2 = argv[1];
	server = gethostbyname(x2); // Get server address
	
	if (!server ) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	char * input1=(char *) &serv_addr;
	int input2 = sizeof(serv_addr);
	bzero(input1, input2);  // Filling in various fields of serv_addr
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	int val =connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) ;
	if (val < 0){ //Initiate connection to server
		error("ERROR connecting");
	}
	
	struct mail msg;
	char temp[MSG_LEN-1];
	char* in1=buffer;
	int in2 =MSG_LEN;
	bzero(in1,in2);

	n = read(sockfd,in1,in2);
	
	if (n <= -1){
		error("ERROR reading from socket");
	}

	printf("%s\n",in1);
	int true =1;
	while(true){
		bzero(in1,in2);
		printf("Please enter the message : \n");  
		fgets(in1,in2,stdin);
		  // Get message to be sent from stdin
		int l = strlen(in1);
		in1[l-1]='\0'; // Delete carriage return character
		msg.message_type=in1[0] - '0'; //First byte is message type
		strcpy(msg.message,in1+1);

		if(msg.message_type == true){ //Send Message
			char *to_send=(char*)malloc((sizeof(char)*MSG_LEN*4+6)/3);
			to_send[0]='1';
			base(in1+1,to_send+1);      //Obtain encoded message in to_send
			int l1 = strlen(to_send);
			n = write(sockfd,to_send,l1); 
			free(to_send);
		
			if (n <= -1){
				error("ERROR writing to socket");  
			}	
		
			bzero(in1,MSG_LEN);    
			n = read(sockfd,in1,MSG_LEN);	
			printf("%s\n",in1);
		}
		
		else if(msg.message_type == 3){ //Close connection
			int x =strlen(in1);
			n = write(sockfd,in1,x);

			if (n <= -1){
				error("ERROR writing to socket");
			}

			break;	
		}
		
		else{
			printf("Start the input with either 1 or 3\n");
			continue;
		}
	}
	close(sockfd);     // Close socket
	free(in1);	// Deallocate buffer space
	return 0;
}

