//
//  main.c
//  client
//
//  Created by Tom Rawlinson on 08/03/2016.
//  Copyright © 2016 Tom Rawlinson. All rights reserved.
//

//Create A socket, and use the inputs to stream to the server


#include<stdio.h> /*printf*/
#include<stdlib.h> /*printf*/
#include<string.h>    /*strlen*/
#include<sys/socket.h>    /*socket*/
#include<arpa/inet.h> /*inet_addr*/
#include <unistd.h>

#define DEBUG

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char  message[1000] , server_reply[2000];
    
    /*Store Ip Address and Command*/
    /*IP is ARGV[1]; CMD is ARGV[2]*/
    printf("argv 1 and 2  = %s %s\n", argv[1], argv[2]);
    
    
    
    
    /*Create socket*/
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        perror("Could not create socket");
        exit(1);
    }
    puts("Socket created");
    
    //server.sin_addr.s_addr = inet_addr("163.1.140.60");
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8891 );
    
    /*Connect to "remote" server*/
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        exit(1);
    }
    
    puts("Connected\n");
    //Keep Talking
    /*keep communicating with server*/
    while(1)
    {
        printf("Enter message : ");
        if(fgets(message,1000,stdin)==NULL)
            message[0]=0;
        //fgets(0,"%[^\n]s", message);
#ifdef DEBUG
        printf("message length = %d\n",(int)strlen(message));
#endif
        message[strlen(message)+1] = 0;
        int message_length = 0;
        int total_sent = 0;
        int sent_now = 0;
        int total_received = 0;
        int received_now = 0;
        
        message_length = (int)strlen(message);
        /*Send message length as int*/
        if( send(sock, &message_length, sizeof(int), 0) != sizeof(int)) {
            perror("Send of message length failed");
            return 1;
        }
        
        if(message_length ==0 ) {
            close(sock);
            return 0;
        }
        /*Send message as mesg length chars*/
        while(total_sent != message_length)
        {
            sent_now= send(sock , message+total_sent*sizeof(char), strlen(message)-total_sent , 0);
            if(sent_now < 0){
                perror("Send failed");
                return 1;
            }
            total_sent +=sent_now;
        }
        
        
        /*Receive message length as int*/
        if(recv(sock, &message_length, sizeof(int), 0) != sizeof(int)) {
            perror("Receipt of message length failed");
            return 1;
        }
        
        if(message_length > (2000-1)) {
            perror("Message sent back is too large for buffer.");
            return 1;
        }
        
        /*Receive message reply from the server*/
        while(total_received != message_length)
        {
            received_now= recv(sock , server_reply+total_received*sizeof(char), message_length-total_received , 0);
            if(received_now < 0) {
                perror("Receive failed");
                return 1;
            }
            total_received +=received_now;
        }
        server_reply[message_length]=0;
        
        puts("Server reply :");
        puts(server_reply);
        
    }
    return 0;
    
}
