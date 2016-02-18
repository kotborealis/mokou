#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <vector>

#define BufferLength 256

using namespace std;

class Server{
	public:
		Server(int sp);
	private:
		int server_port;//port
		int listener, clientfd;//server listen socket descriptor, accepted socket descriptor
		fd_set master;//master descriptor list
		fd_set read_fds, write_fds;//temp descriptor lists
		int fdmax;//max descriptor number (for select())
		char buf[BufferLength];//buffer
		int nBytes;//temp var for read
		struct sockaddr_in server_addr, remote_addr;//server client addr info
		socklen_t addrlen;
		vector<uint8_t> wbuf={};

		void initSocket(int *socket_desc, struct sockaddr_in *server_addr, int port);
		void connection_handler(int *socket_desc, struct sockaddr_in *remote_addr);
		void disconnect_handler(int *socket_desc);
		void data_handler(int *socket_desc,char *buf, int length);
		void poll();
		void error(const char *msg);
};
