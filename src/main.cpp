#include "server.h"
#include "websockets.h"
#include "chat.h"

using namespace std;

int main(int argc, char** argv){
	int port=8090;
	if(argc>=2)
		port=atoi(argv[1]);

	Chat s = Chat(port);
	s.poll();
	return 0;
}