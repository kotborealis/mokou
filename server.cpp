#include "server.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

using namespace std;

Server::Server(int sp){
	server_port = sp;
	initSocket(&listener,&server_addr,server_port);
	listen(listener,5);
	cout<<"Listening on " << server_port << "\n";
	FD_SET(listener,&master);
	cout<<"Listener:"<<listener<<"\n";
	fdmax = listener;

	int i,j;
	for(;;){
		read_fds=master;
		cout<<"copied\n";
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1)
			error("Error: Server::Server select error");
		for(i=listener;i<=fdmax;i++){
			if(FD_ISSET(i,&read_fds)){
				if(i==listener){//new connection
					cout<<"new connection\n";
					addrlen = sizeof(remote_addr);
					clientfd = accept(listener,(struct sockaddr*)&remote_addr,&addrlen);

					if(clientfd==-1){
						perror("Error: can't accept client");
					}
					else{
						FD_SET(clientfd,&master);
						if(clientfd>fdmax)
							fdmax=clientfd;
						cout<<"new connection success\n";
						send(clientfd,"sassos",6,0);
						cout<<"Client:"<<clientfd<<"\n";
					}
				}
				else{//data from client
					cout<<"data!\n";
					if((nBytes = recv(i, buf, sizeof(buf), 0))<=0){//connection closed/lost/error
						if(nBytes==0)
							cout<<"Connection closed";
						else
							perror("Error reading data from socket");
						close(i);
						FD_CLR(i,&master);
					}
					else{//data
						cout<<"{\n " << buf << "}\n";
						for(j=listener+1;j<fdmax;j++){
							if(FD_ISSET(j,&master)){
								if(j!=listener && j!=i){
									if(send(j,buf,nBytes,0)==-1)
										perror("send");
								}
							}
						}
					}
				}
		}
			}
	}
	//int c = sizeof(struct sockaddr_in);
}

void Server::initSocket(int *sockfd, struct sockaddr_in *server_addr, int port){
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(*sockfd == -1)
		error("Error: Server::initSocket socket creation failed\n");

	int enable=1;
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&enable, (socklen_t) sizeof(enable)) < 0)
    	error("Error: Server::initSOcket setsockopt(SO_REUSEADDR) failed");

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