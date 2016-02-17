

using namespace std;

#define BufferLength 256

Server::Server(int server_port){
	int sockfd, newsockdf, clilen, n;
	char buffer[BufferLength];
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socketc(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)
		error("sockdf error");

}

void Server::error(const char *msg){
    perror(msg);
    exit(1);
}