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

	FD_SET(listener,&master);
	fdmax = listener;
}
void Server::poll() {
	int i;
	for(;;){
		read_fds=master;
		write_fds=master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1)
			error("Error: Server::Server select errorn\n");
		for(i=listener;i<=fdmax;i++){
			if(FD_ISSET(i,&read_fds)){
				if(i==listener){//new connection
					addrlen = sizeof(remote_addr);
					clientfd = accept(listener,(struct sockaddr*)&remote_addr,&addrlen);
					if(clientfd==-1){
						perror("Error: can't accept client\n");
					}
					else{
						FD_SET(clientfd,&master);
						if(clientfd>fdmax)
							fdmax=clientfd;
						this->connection_handler(&clientfd, &remote_addr);
					}
				}
				else{//data from client
					if((nBytes = recv(i, buf, sizeof(buf), 0))<=0){//connection closed/lost/error
						if(nBytes==0){
							
						}
						else
							perror("Error: reading data from socket\n");
						this->disconnect_handler(&i);
						close(i);
						FD_CLR(i,&master);
					}
					else{//data
						this->data_handler(&i,buf,nBytes);
					}
				}
			}
			if(FD_ISSET(i,&write_fds) && i!=listener){
				vector<uint8_t> _wbuf = wbuf;
				while(_wbuf.size()){
					int ret=send(i,(char*)&_wbuf[0],_wbuf.size(),0);
					if(ret<0){
						error("Cannot write data to socket");
					}
					else if(ret==0){
						error("Socket closed!");
					}
					else{
						_wbuf.erase(_wbuf.begin(),_wbuf.begin()+ret);
					}
				}
			}
		}
		wbuf.erase(wbuf.begin(),wbuf.end());
	}
}
/*
void Server::connection_handler(int *socket_desc, struct sockaddr_in *remote_addr){
	//cout<<"Connected: "<<*socket_desc<<"@"<<inet_ntoa(remote_addr->sin_addr)<<"\n";
	cout<<"WHAT\n";
	send(*socket_desc,"sas",3,0);
}
void Server::disconnect_handler(int *socket_desc){
	//cout<<"Disconnected: "<<*socket_desc<<"\n";
}
void Server::data_handler(int *socket_desc,char *buf, int length){
	if(*buf=='z'){
		//"\r\nZA WARUDO! TOKI WA TOMARE\r\n"
		string str = "\r\nArar rar\r\n";
		cout<<"Rar\n";
		wbuf.insert(wbuf.end(),str.begin(),str.end());
	}
}*/

void Server::error(const char *msg){
    perror(msg);
    exit(1);
};