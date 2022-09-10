# Base64-Encoding-based-Client-Server-Communication-System


 **#######     BASE64 encoding system using Client-Server socket programming     #######**

* server can handle many clients simultaneously and while communicating it uses base64 encoding.
* client server communication work on TCP sockets.
* Initially server will be waiting for the client to create TCP connection.
* Then client will communicate with the server on already known port number. 
* After succesfull connection establishment, client will ask input message from user and and client encodes this message and send to the server.
* After recieving the message , server decodes the message and prints the original message and decoded message. and server sends an ACK message to the client.
* Client and server remains in the loops to continue the endless communication, untill the user want to terminate the connection.
* If client wants to close the connection, client send the type 3 (close connection) message to the server.
* After recieving close connection message from client, TCP connection is closed and all the socket resources are released successfully.

## HOW TO EXECUTE THE PROGRAM

This module consists of 4 file-:
* **client.c**                  //this file includes the encode and decode module.
* **server.c**                  //this file also includes the encode and decode module.
* **encode.c**                  //Encoding the text message to base64 encoding
* **decode.c**                  //Decoding the base64 encoded message.

##  For compilation of the client.c:
* **Command :- **        gcc -o client client.c -lm

##  For compilation of the server.c:
* **Command :-**        gcc -o server server.c -lm

##  To start the server :
          ./server 9090
          
##  To start the client :
          ./client 127.0.0.1 9090
       
##  To start the another client :
          ./client 127.0.0.1 9090
          
##  Prototypes for Client and Server :

        > Client: _<executable code><Server IP Address><Server Port number>_
        > Server: _<executable code><Server Port number>_
        
