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
	
}
void Websockets::disconnect_handler(int socket_desc){
	
}
void Websockets::data_handler(int socket_desc, char *buf, int length){
	buf[255]='\0';
	unsigned char sha1buf[20], sha1res[20];
	string handshakeKey(buf);
	size_t keyIndex=handshakeKey.find("Sec-WebSocket-Key: ");
	cout<<buf;
	if(keyIndex!=string::npos){
		handshakeKey=handshakeKey.substr(keyIndex+19,24);
		handshakeKey=handshakeKey+websocketMagicString; 
		sha1::calc((void*)handshakeKey.c_str(),handshakeKey.length(),(unsigned char*)sha1buf);
		handshakeKey=base64_encode((const unsigned char*)sha1buf,20);
		cout<<"\nda: "<<handshakeKey<<"\n";
		/*
		HTTP/1.1 101 Switching Protocols
        Upgrade: websocket
        Connection: Upgrade
        Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
        Sec-WebSocket-Protocol: chat
		*/
		string handshakeHeaders="HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
		handshakeHeaders=handshakeHeaders+handshakeKey;
		handshakeHeaders=handshakeHeaders+"\r\n\r\n\r\n";
		cout<<handshakeHeaders;
		send(socket_desc,(char*)handshakeHeaders.c_str(),handshakeHeaders.size(),0);
	}
	/*
	close(socket_desc);
	    clients.erase(socket_desc);
	    disconnect_handler(socket_desc);
		return;
		*/
}
/*int Websockets::read_handler(int *socket_desc){
	int sockfd=*socket_desc;
	string data_;
	if(readyState==CONNECTING){
		int i;
		for(i=0;i<BufferSize-1 && !(data[i-2]=='\r' && data[i-1]=='\n');i++){
			if(recv(sockfd,data+i,1,0)==0)
				break;
		}
		cout<<data;
		if(data[0]=='\r' && data[1]=='\n'){
			readyState=CLOSED;
			cout<<"rnrn"<<sockfd<<"\n";
			close(sockfd);
			FD_CLR(sockfd,&master);
		}
	}
	return 0;
}*/