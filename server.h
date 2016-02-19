#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string.h>
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
		void poll();
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

		void initSocket(int *socket_desc, struct sockaddr_in *server_addr, int port);
	protected:
		void error(const char *msg);
		vector<uint8_t> wbuf={};

		virtual void connection_handler(int *socket_desc, struct sockaddr_in *remote_addr)=0;
		virtual void disconnect_handler(int *socket_desc)=0;
		virtual void data_handler(int *socket_desc,char *buf, int length)=0;
};

#endif