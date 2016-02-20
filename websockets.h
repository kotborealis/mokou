#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include "server.h"

#include <vector>

class Websockets: public Server{
	public:
		Websockets(int sp);
	private:
		string websocketMagicString="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		string generateHandshakeKey(string key);
	protected:
		typedef enum readyStateValues { CLOSING, CLOSED, CONNECTING, OPEN } readyStateValues;

		typedef struct ws_client{
			string ip;
			readyStateValues readyState;
		} ws_client;

		vector<ws_client> ws_clients = vector<ws_client>(256);

		void connection_handler(int socket_desc, struct sockaddr_in *remote_addr);
		void disconnect_handler(int socket_desc);
		void data_handler(int socket_desc, char *buf, int length);
};

#endif