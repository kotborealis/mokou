#include "server.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <set>

using namespace std;

Server::Server(int sp){
	server_port = sp;
	initSocket(&listener,&server_addr,server_port);
	listen(listener,5);
	cout<<"Server listening!\n";
}

void Server::initSocket(int *sockfd, struct sockaddr_in *server_addr, int port){
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(*sockfd == -1)
		error("Error: Server::initSocket socket creation failed\n");

	int enable=1;
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&enable, (socklen_t) sizeof(enable)) < 0)
    	error("Error: Server::initSocket setsockopt(SO_REUSEADDR) failed");
    if (fcntl(*sockfd, F_SETFL, O_NONBLOCK)<0)
    	error("Error: Server::initSocket fcntl nonblock failed");

	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = INADDR_ANY;
	server_addr->sin_port = htons(port);

	if(bind(*sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr))<0)
		error("Error: Server::initSocket bind sockfd;\n");
}
void Server::poll() {
	for(;;){
		FD_ZERO(&read_fds);
		FD_SET(listener,&read_fds);

		for(set<int>::iterator it = disconnect.begin(); it != disconnect.end(); it++){
            clients.erase(*it);
		}
		disconnect.clear();

		for(set<int>::iterator it = clients.begin(); it != clients.end(); it++)
            FD_SET(*it, &read_fds);

        fdmax=max(listener, *max_element(clients.begin(), clients.end()));

		if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1)
			error("Error: Server::Server select errorn\n");

		if(FD_ISSET(listener,&read_fds)){
			addrlen = sizeof(remote_addr);
			clientfd = accept(listener,(struct sockaddr*)&remote_addr,&addrlen);
			if(clientfd==-1){
				perror("Error: can't accept client\n");
			}
			else{
				fcntl(clientfd, F_SETFL, O_NONBLOCK);
				clients.insert(clientfd);
				connection_handler(clientfd, &remote_addr);
			}
		}
		for(set<int>::iterator it = clients.begin(); it != clients.end(); it++)
        {
            if(FD_ISSET(*it, &read_fds))
            {
                bytes_read = recv(*it, buf, BufferSize, 0);

                if(bytes_read <= 0)
                {
                    disconnect_handler(*it);
                    continue;
                }
                data_handler(*it,buf,bytes_read);
            }
    	}
	}
}

void Server::error(const char *msg){
    perror(msg);
    exit(1);
};

void Server::closesock(int sockfd){
	close(sockfd);
    disconnect.insert(sockfd);
    FD_CLR(sockfd,&read_fds);
}