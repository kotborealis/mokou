#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include "server.h"

class Websockets: public Server{
	public:
		Websockets(int sp);
	private:
		int handshakeState=0;
		string websocketMagicString="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	protected:
		typedef enum readyStateValues { CLOSING, CLOSED, CONNECTING, OPEN } readyStateValues;
		readyStateValues readyState = CONNECTING;

		void connection_handler(int socket_desc, struct sockaddr_in *remote_addr);
		void disconnect_handler(int socket_desc);
		void data_handler(int socket_desc, char *buf, int length);
};

#endif