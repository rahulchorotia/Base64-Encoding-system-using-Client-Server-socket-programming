#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<math.h>


// The following function returns Base64 character corresponding to it's integer value 

char map_int_to_char(int ch)
{
    char c;
	if(ch<=25)
	{
	    c=ch+65;	// 'A'-'Z'
	}
	else if(ch<=51)
	{
	    c=ch+71;	// 'a'-'z'
	}
	else if(ch<=61)
	{
	    c=ch-4;  	// '0'-'9'
	}
	else if(ch==62)
		c='+';		// '+'
	else
		c='/';
		
	return c;		// '/'
}

// The following function takes input string of 3 characters and returns binary string of size 24
void char_to_binary(char* message,char* out)
{
	int i,j;
	for(i=0;i<3;i++)
	{
		if(message[i]==0)
		{
			for(j=i*8;j<(i+1)*8;j++)
				out[j]='0';
		}
		else
		{
			int ascii_value=message[i];
			for(j=7;j>=0;j--)
			{
				if(ascii_value>=pow(2.0,j))
				{
					ascii_value-=pow(2.0,j);
					out[i*8+7-j]='1';
				}
				else
					out[i*8+7-j]='0';
			}
		}
	}
}

// The following function takes input binary string of size 24 and returns string of characters of size 4 by forming group of 6 binary at a time
void binary_to_en_message(char* Bin_message,int padding,char* out,int last)
{
	int i,j,x;
	for(i=0;i<4;i++)
	{
		if(last && i>=(4-padding))
		{
			out[i]='=';
		}
		else
		{
			x=0;
		    for(j=0;j<6;j++)
			{
			   x+=((int)(Bin_message[i*6+j]-'0'))*pow(2.0,5-j);
		    }
		    //printf("%d",x);
		   out[i]=map_int_to_char(x);
		}
		
	}
}

// the following function encodes original message to Base64 message
char* encode(char* message)
{
	int i,j,k,pad,last=0;
	char temp[4],temp1[5],b[25];
	temp[3]=0;
        temp1[4]=0;
        b[24]=0;
	int l=strlen(message);
	pad=3-((l-1)%3)-1;
	// printf("%d\n", pad);
	char* op=(char *)malloc(sizeof(char)*2*l);
	
	for(i=0,j=0;i<l;i+=3,j++)
	{
		if((i+3)<l)
		{
			temp[0]=message[i+0]; 
			temp[1]=message[i+1];
			temp[2]=message[i+2];
		}
		else
		{
			last=1;
			for(k=0;k<3;k++)
			{
				if(k>=(3-pad))
				  temp[k]=0;
				else
				  temp[k]=message[i+k];
			}
		}
	//	 printf("%s\n",temp);
		char_to_binary(temp,b);
	//	 printf("%s\n",b);
		binary_to_en_message(b,pad,temp1,last);
	//	printf("%s\n",temp1);
		for(k=0;k<4;k++){
			op[4*j+k]=temp1[k];
		}
	}
	op[4*j]='\0';
	return(op);
}

