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

#define BufferLength 256

using namespace std;

class Server{
	public:
		vector<int> v_clients;
		Server(int sp);
	private:
		int server_port;
		int sockfd, clientfd;
		struct sockaddr_in server_addr, client_addr;

		void initSocket(int *socket_desc, struct sockaddr_in *socket, int port);
		void error(const char *msg);
};