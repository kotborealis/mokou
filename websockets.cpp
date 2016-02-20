#include "websockets.h"
#include "sha1.h"
#include "base64.h"
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
#include <cstdlib>

Websockets::Websockets(int sp): Server(sp){}

void Websockets::connection_handler(int socket_desc, struct sockaddr_in *remote_addr){
	ws_clients[socket_desc].ip=inet_ntoa(remote_addr->sin_addr);
	ws_clients[socket_desc].readyState=CONNECTING;
	ws_on_connect(socket_desc);
}
void Websockets::disconnect_handler(int socket_desc){
	ws_on_close(socket_desc);
}
void Websockets::data_handler(int socket_desc, unsigned char *buf, int length){
	if(ws_clients[socket_desc].readyState==CONNECTING){
		string handshakeKey(reinterpret_cast<const char*>(buf));
		size_t keyIndex=handshakeKey.find("Sec-WebSocket-Key: ");
		if(keyIndex!=string::npos){
			handshakeKey=handshakeKey.substr(keyIndex+19,24);
			handshakeKey=generateHandshakeKey(handshakeKey);
			string handshakeHeaders="HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
			handshakeHeaders=handshakeHeaders+handshakeKey;
			handshakeHeaders=handshakeHeaders+"\r\n\r\n";
			send(socket_desc,(char*)handshakeHeaders.c_str(),handshakeHeaders.size(),0);
			ws_clients[socket_desc].readyState=OPEN;
			return;
		}
	}
	else if(ws_clients[socket_desc].readyState==OPEN){
		websocketHeader ws=parse_header(buf);
	}
	else if(ws_clients[socket_desc].readyState==CLOSING){
		
		return;
	}
	else if(ws_clients[socket_desc].readyState==CLOSED){
		//closed 
		return;
	}
}

string Websockets::generateHandshakeKey(string key){
	unsigned char sha1buf[20];
	key=key+websocketMagicString;
	sha1::calc((void*)key.c_str(),key.length(),(unsigned char*)sha1buf);
	key=base64_encode((const unsigned char*)sha1buf,20);
	return key;
}

void Websockets::ws_send(int sockfd, string &data){
	vector<unsigned char> send_buf;//FIN, RSV1-3, OPCODE, LENGTH-7bit(0-125)
	send_buf.push_back(0x81);//1 000 0001 - fin text
	
	if((data.length())<=125){
		send_buf.push_back(data.length());
	}
	else if((data.length())<=65535){
		send_buf.push_back(126);
		send_buf.push_back(data.length()&0xFF);
		send_buf.push_back((data.length()>>8)&0xFF);
	}
	else{
		return;
	}
	for(auto it = data.begin(); it!=data.end(); ++it){
		send_buf.push_back(*it);
	}
	send(sockfd,&send_buf[0],send_buf.size(),0);
}


void Websockets::ws_send_binary(int sockfd, string &data){
	vector<unsigned char> send_buf;//FIN, RSV1-3, OPCODE, LENGTH-7bit(0-125)
	send_buf.push_back(0x82);//1 000 0010 - fin binary
	
	if((data.length())<=125){
		send_buf.push_back(data.length());
	}
	else if((data.length())<=65535){
		send_buf.push_back(126);
		send_buf.push_back(data.length()&0xFF);
		send_buf.push_back((data.length()>>8)&0xFF);
	}
	else{
		return;
	}
	for(auto it = data.begin(); it!=data.end(); ++it){
		send_buf.push_back(*it);
	}
	send(sockfd,&send_buf[0],send_buf.size(),0);
}

void Websockets::ws_send_close(int sockfd){
	vector<unsigned char> send_buf;//FIN, RSV1-3, OPCODE, LENGTH-7bit(0-125)
	send_buf.push_back(0x83);//1 000 0011 - close
	send_buf.push_back(0);
	send(sockfd,&send_buf[0],send_buf.size(),0);
}

void Websockets::ws_send_ping(int sockfd){
	vector<unsigned char> send_buf;//FIN, RSV1-3, OPCODE, LENGTH-7bit(0-125)
	send_buf.push_back(0x84);//1 000 0100 - ping
	send_buf.push_back(0);
	send(sockfd,&send_buf[0],send_buf.size(),0);
}
void Websockets::ws_send_pong(int sockfd){
	vector<unsigned char> send_buf;//FIN, RSV1-3, OPCODE, LENGTH-7bit(0-125)
	send_buf.push_back(0x85);//1 000 0101 - pong
	send_buf.push_back(0);
	send(sockfd,&send_buf[0],send_buf.size(),0);
}

Websockets::websocketHeader Websockets::parse_header(unsigned char *data){
	websocketHeader ws;
	cout<<"\n-START-----------------\n"<<data<<"\n-END------------------------------------";
	return ws;
}