//
//  main.c
//  server
//
//  Created by Tom Rawlinson on 08/03/2016.
//  Copyright © 2016 Tom Rawlinson. All rights reserved.
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

#define DEBUG

int main(int argc , char *argv[])
{
    int socket_fd, client_sock, c, read_size;
    struct sockaddr_in server, client;
    char client_message[2000];
    int message_length;
    int total_received;
    int received_now;
    
    /* Initialize socket */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("Socket not created.");
        exit(1);
    }
    puts("Socket created");
    
    /* Prepare sockaddr_in structure */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8891 );
    
    /* Bind socket to port */
    if(bind(socket_fd,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        exit(1);
    }
    puts("Server socket bound to port 8891");
    
    /* Indicate that we will be listening on this socket */
    listen(socket_fd , 3);
    
    /* prepare to accept incoming connection */
    puts("Waiting for incoming connections...");
    client_sock = accept(socket_fd, (struct sockaddr *)&client, (socklen_t*)&c);
    
    if (client_sock < 0)
    {
        perror("accept failed");
        exit(1);
    }
    puts("Connection accepted");
    
    if(recv(client_sock, &message_length, sizeof(int), 0) != sizeof(int)) {
        perror("Receipt of message length failed");
        return 1;
    }
    while( message_length > 0) {
        char * return_message;
        int num_written = 0;
        int total_written = 0;
        
        
#ifdef DEBUG
        printf("allocating space for message length %d\n",message_length);
#endif
        return_message = malloc((message_length+1)*sizeof(char));
        
        total_received = 0;
        received_now = 0;
        while(total_received != message_length) {
            received_now= recv(client_sock
                               , return_message+total_received*sizeof(char), message_length-total_received
                               , 0);
            if(received_now < 0) {
                perror("Receive failed");
                return 1;
            }
            total_received +=received_now;
        }
#ifdef DEBUG
        printf("Server received message %s from client\n", return_message);
#endif
        
        /* do something (capitalize) and send the message back to client */
        for(int i = 0;i < message_length; i++)
            return_message[i] = toupper(return_message[i]);
        
        if( send(client_sock, &message_length, sizeof(int), 0) != sizeof(int)) {
            perror("Send of message length failed");
            return 1;
        }
        
        while(total_written < message_length) {
            num_written = send(client_sock,
                               return_message+total_written*sizeof(char) , message_length-total_written,0);
            if (num_written < 0) {
                perror("Error writing return message, shutting server down");
                exit(1);
            }
            total_written+=num_written;
#ifdef DEBUG
            puts("writing return message to socket");
#endif
        }
        /* check out next message length (another int) */
        if(recv(client_sock, &message_length, sizeof(int), 0) != sizeof(int)) {
            perror("Receipt of message length failed"); 
            return 1; 
        }
#ifdef DEBUG
        printf("Server receiving another message of length %d\n",message_length);
#endif
        free(return_message); 
    }
    
    puts("Client disconnected");
    fflush(stdout);
    close(client_sock); 
    
    
    return 0;
}


