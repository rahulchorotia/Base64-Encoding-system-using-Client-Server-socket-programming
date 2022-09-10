#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<math.h>


// The following function maps char to there base64 integer value

int map_char_to_int(char c)
{
	int v;
	if(c >= 'A' && c <= 'Z' )
	{
		v=(c-'A');			// 0-25
	}
	else if(c >= 'a' && c <= 'z' )
	{
		v=(c-'a'+26);		// 26-51
	}
	else if(c >= '0' && c <= '9' )
	{
		v=(c-'0'+52);		// 52-61
	}
	else if(c=='+')
		v=62;				// 62
	else
		v=63;               // 63

	return v;
}

// The following function takes input base64 string of 4 characters and convert it into binary string of size 24

void base64_to_binary(char* message,char* Bin_out)
{
	int i,j;
	for(i=0;i<4;i++)
	{
		if(message[i]=='=')
		{
			for(j=i*6;j<(i+1)*6;j++)
				Bin_out[j]='0';
		}
		else
		{
			int b64_v=map_char_to_int(message[i]);
			// printf("%d\n", b64_v);
			for(j=5;j>=0;j--)
			{
				if(b64_v>=pow(2.0,j))
				{
					b64_v-=pow(2.0,j);
					Bin_out[i*6+5-j]='1';
				}
				else
					Bin_out[i*6+5-j]='0';
			}
		}
	}
}

// The following function takes 24 bit binary string and convert it into string of 3 char's by forming group of 8 bits
void binary_to_char(char* bin_message,int padding,int flag,char* out){
	int i,j,value;
	for(i=0;i<3;i++){
		if(flag && i>=3-padding){
			break;
		}
		value=0;
		for(j=7;j>=0;j--){
			value+=(bin_message[i*8+7-j]-'0')*pow(2.0,j);
		}
		out[i]=value;
	}
}	

// the following function decodes Base64Encoded message to original message

char* decode(char* en_message)
{
	int i,j,k,pad=0,flag=0,last=0,l=strlen(en_message);
	char temp[5],b[25],temp1[4];
	char* op=(char *)malloc(sizeof(char)*l);
	temp[4]='\0';
	b[24]='\0';
	temp1[3]='\0';
	for(i=0,j=0;i<l;i+=4,j++)
	{
		pad=0;
		for(k=0;k<4;k++)
		{
			if(en_message[i+k]=='=')
			{
				flag=1;
				pad++;
			}
			temp[k]=en_message[i+k];
		}
		//printf("%s\n",temp);
		base64_to_binary(temp,b);
	        // printf("%s\n",b);
		binary_to_char(b,pad,flag,temp1);
		//printf("%s\n",temp1);
		for(k=0;k<3;k++)
		{
			if(flag && k>=3-pad)
			{
				break;
			}
			last=3*j+k+1;
			op[3*j+k]=temp1[k];
		}
	}
	op[last]='\0';
	return(op);
}

