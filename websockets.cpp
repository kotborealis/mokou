#include "websockets.h"

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

void Websockets::connection_handler(int *socket_desc, struct sockaddr_in *remote_addr){
	cout<<"Connected: "<<*socket_desc<<"@"<<inet_ntoa(remote_addr->sin_addr)<<"\n";
	send(*socket_desc,"ass",3,0);
}
void Websockets::disconnect_handler(int *socket_desc){
	cout<<"Disconnected: "<<*socket_desc<<"\n";
}
void Websockets::data_handler(int *socket_desc,char *buf, int length){
	if(*buf=='z'){
		//"\r\nZA WARUDO! TOKI WA TOMARE\r\n"
		string str = "\r\nArar rar\r\n";
		cout<<"Rar\n";
		wbuf.insert(wbuf.end(),str.begin(),str.end());
	}
}

