#include "server.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <pthread.h>

using namespace std;

static void *connection_handler(void *sockfd){
    int sock = *(int*)sockfd;
    cout<<sock<<"\n";
    char *message="faffofsasses";
    write(sock , message , strlen(message));
};

Server::Server(int sp){
	server_port = sp;
	initSocket(&sockfd,&server_addr,server_port);
	listen(sockfd,5);

	cout<<"Listening on " << server_port << "\n";

	int c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	while(clientfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&c)){
		if(pthread_create(&thread_id,NULL,connection_handler,(void*)&clientfd)<0)
			error("Cannot create thread");
		pthread_join(thread_id,NULL);
	}
}

void Server::initSocket(int *sockfd, struct sockaddr_in *server_addr, int port){
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(*sockfd == -1)
		error("Error: Server::initSocket socket creation failed\n");

	int enable=1;
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&enable, (socklen_t) sizeof(enable)) < 0)
    	error("setsockopt(SO_REUSEADDR) failed");

	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = INADDR_ANY;
	server_addr->sin_port = htons(port);

	if(bind(*sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr))<0)
		error("Error: Server::initSocket bind sockfd;\n");
}

void Server::error(const char *msg){
    perror(msg);
    exit(1);
};