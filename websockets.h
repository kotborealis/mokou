#pragma once 

#include "server.h"
#include <map>

class Websockets: public Server{
	public:
		Websockets(int sp);
	private:
		string websocketMagicString="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		typedef enum opcode_type {
			CONTINUATION = 0x0,
            TEXT_FRAME = 0x1,
            BINARY_FRAME = 0x2,
            CLOSE = 0x8,
            PING = 0x9,
            PONG = 0xa,
		} opcode_type;

		struct websocketHeader{
			bool fin;
			bool mask;
			opcode_type opcode;
			int size;
			uint8_t masking_key[4];
		};

		void connection_handler(int socket_desc, struct sockaddr_in *remote_addr);
		void disconnect_handler(int socket_desc);
		void data_handler(int socket_desc, unsigned char *buf, int length);
		websocketHeader parse_header(unsigned char *data);
		string generateHandshakeKey(string key);
	protected:
		typedef enum readyStateValues { CLOSING, CLOSED, CONNECTING, OPEN } readyStateValues;

		typedef struct ws_client{
			string ip;
			readyStateValues readyState;
		} ws_client;

		map<int,ws_client> ws_clients;

		void ws_send(int sockfd, string &data);
		void ws_send_binary(int sockfd, string &data);
		void ws_send_close(int sockfd);
		void ws_send_ping(int sockfd);
		void ws_send_pong(int sockfd);

		virtual void ws_on_connect(int clientid){};
		virtual void ws_on_close(int clientid){};
		virtual void ws_on_message(int clientid, string &message){};
		virtual void ws_on_data(int clientid){};
};