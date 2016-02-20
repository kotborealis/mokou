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

Websockets::Websockets(int sp): Server(sp){}

void Websockets::connection_handler(int socket_desc, struct sockaddr_in *remote_addr){
	cout<<"Connection handler?\n";
	ws_clients[socket_desc].ip=inet_ntoa(remote_addr->sin_addr);
	ws_clients[socket_desc].readyState=CONNECTING;
	cout<<ws_clients[socket_desc].ip<<"@"<<":"<<ws_clients[socket_desc].readyState<<"\n";
}
void Websockets::disconnect_handler(int socket_desc){
	
}
void Websockets::data_handler(int socket_desc, char *buf, int length){
	if(ws_clients[socket_desc].readyState==CONNECTING){
		string handshakeKey(buf);
		size_t keyIndex=handshakeKey.find("Sec-WebSocket-Key: ");
		if(keyIndex!=string::npos){
			handshakeKey=handshakeKey.substr(keyIndex+19,24);
			handshakeKey=generateHandshakeKey(handshakeKey);
			string handshakeHeaders="HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
			handshakeHeaders=handshakeHeaders+handshakeKey;
			handshakeHeaders=handshakeHeaders+"\r\n\r\n";
			send(socket_desc,(char*)handshakeHeaders.c_str(),handshakeHeaders.size(),0);
			ws_clients[socket_desc].readyState=OPEN;
			unsigned char a[7]={0x81,0x05,0x42,0x6f,0x72,0x69,0x73};
			send(socket_desc,&a[0],7,0);
			return;
		}
	}
	else if(ws_clients[socket_desc].readyState==OPEN){
		
	}
	else if(ws_clients[socket_desc].readyState==CLOSING){
		//closing
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