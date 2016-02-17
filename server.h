#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define BufferLength 256

using namespace std;

class Server{
	public:
		Server(int sp);
	private:
		int server_port;//port
		int listener, clientfd;//server listen socket descriptor, accepted socket descriptor
		fd_set master;//master descriptor list
		fd_set read_fds;//temp descriptor list
		int fdmax;//max descriptor number (for select())
		char buf[BufferLength];//buffer
		int nBytes;//temp var for read
		struct sockaddr_in server_addr, remote_addr;//server client addr info
		socklen_t addrlen;

		void initSocket(int *socket_desc, struct sockaddr_in *socket, int port);
		void error(const char *msg);
};