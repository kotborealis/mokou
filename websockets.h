#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include "server.h"

class Websockets: public Server{
	public:
		Websockets(int sp);
	protected:
		void connection_handler(int *socket_desc, struct sockaddr_in *remote_addr);
		void disconnect_handler(int *socket_desc);
		void data_handler(int *socket_desc,char *buf, int length);
};

#endif