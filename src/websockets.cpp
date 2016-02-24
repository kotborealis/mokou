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

using namespace std;

Websockets::Websockets(int sp): Server(sp){}

void Websockets::connection_handler(int socket_desc, struct sockaddr_in *remote_addr){
	ws_clients[socket_desc].ip=inet_ntoa(remote_addr->sin_addr);
	ws_clients[socket_desc].readyState=CONNECTING;
}
void Websockets::disconnect_handler(int socket_desc){
	closesock(socket_desc);
	ws_on_close(socket_desc);
}
void Websockets::data_handler(int socket_desc, unsigned char *buf, unsigned int length){
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
			for(;buf[length-1]!='\n' && 
				 buf[length-2]!='\r' && 
				 buf[length-3]!='\n' && 
				 buf[length-4]!='\r';length=recv(socket_desc, buf, BufferSize, 0));//Wait for \r\n\r\n
			ws_on_connect(socket_desc);
			return;
		}
		return;

	}
	else if(ws_clients[socket_desc].readyState==OPEN){
		websocketHeader ws=parse_header(buf,length);
		if(ws.opcode==TEXT_FRAME || ws.opcode==BINARY_FRAME){
			ws_on_message(socket_desc,read_data(socket_desc,ws,buf,length));
		}
		else if(ws.opcode==PING){
			ws_send_pong(socket_desc);
		}
		else if(ws.opcode==PONG){
			
		}
		else if(ws.opcode==INVALID){
			return;
		}
	}
	else if(ws_clients[socket_desc].readyState==CLOSING){
		disconnect_handler(socket_desc);
		return;
	}
	else if(ws_clients[socket_desc].readyState==CLOSED){
		disconnect_handler(socket_desc);
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

void Websockets::ws_send(int sockfd, string data){
	vector<unsigned char> send_buf;//FIN, RSV1-3, OPCODE, LENGTH-7bit(0-125)
	send_buf.push_back(0x81);//1 000 0001 - fin text
	
	if((data.length())<=125){
		send_buf.push_back(data.length());
	}
	else if((data.length())<=65535){
		send_buf.push_back(126);
		send_buf.push_back((data.length()>>8)&0xFF);
		send_buf.push_back(data.length()&0xFF);
	}
	else{
		return;
	}
	for(auto it = data.begin(); it!=data.end(); ++it){
		send_buf.push_back(*it);
	}
	send(sockfd,&send_buf[0],send_buf.size(),0);
}


void Websockets::ws_send_binary(int sockfd, string data){
	vector<unsigned char> send_buf;//FIN, RSV1-3, OPCODE, LENGTH-7bit(0-125)
	send_buf.push_back(0x82);//1 000 0010 - fin binary
	
	if((data.length())<=125){
		send_buf.push_back(data.length());
	}
	else if((data.length())<=65535){
		send_buf.push_back(126);
		send_buf.push_back((data.length()>>8)&0xFF);
		send_buf.push_back(data.length()&0xFF);
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

void Websockets::ws_broadcast(string data){
	for(map<int,ws_client>::iterator it=ws_clients.begin();it!=ws_clients.end();++it){
		Websockets::ws_send(it->first,data);
	}
}
void Websockets::ws_broadcast_binary(string data){
	for(map<int,ws_client>::iterator it=ws_clients.begin();it!=ws_clients.end();++it){
		Websockets::ws_send(it->first,data);
	}
}

Websockets::websocketHeader Websockets::parse_header(unsigned char *data, unsigned int length){
	unsigned int offset=0;
	websocketHeader ws;

	if(length<2){
		ws.opcode=(opcode_type)INVALID;
		return ws;
	}

	ws.fin=data[0]>>0x7;
	ws.opcode=(opcode_type)(data[0]&0xf);
	if(ws.opcode>0xa){
		ws.opcode=(opcode_type)INVALID;
		return ws;
	}

	ws.mask=data[1]>>0x7;
	ws._size=data[1]&0x7f;
	offset+=2;

	ws.size=0;
	if(ws._size==126){
		for(size_t i=0;i<2;i++)
			ws.size|=((uint64_t)data[offset+i])<<((1-i)*8);
		offset=4;
	}
	else if(ws._size==127){
		ws.size=0;
		ws.opcode=(opcode_type)INVALID;
		return ws;
	}
	else if(ws._size<126){
		ws.size=ws._size;
	}
	if(ws.mask){
		for(size_t i=0;i<4;i++)
			ws.masking_key[i]=data[offset+i];
		offset+=4;
	}
	else{
		for(size_t i=0;i<4;i++)
			ws.masking_key[i]=0;
	}
	ws.header_size=offset;
	return ws;
}

string Websockets::read_data(int socket_desc, websocketHeader ws, unsigned char *buf, int length){
	string ws_r_data;
	string ws_r_data2;

	size_t i;
	for(i=0;i<ws.size-(ws.size-(length-ws.header_size));i++){
		ws_r_data+=buf[ws.header_size+i]^ws.masking_key[i&0x3];
	}

	while(length-ws.header_size<ws.size){
		size_t bytes_read=recv(socket_desc, buf, BufferSize, 0);
		for(size_t j=0;j<bytes_read;j++){
			ws_r_data2+=buf[j]^ws.masking_key[(i+j)&0x3];
		}
		ws_r_data+=ws_r_data2;
		ws_r_data2.clear();
		length+=bytes_read;
	}
	return ws_r_data;
}
