#include "server.h"
#include "websockets.h"
#include "chat.h"

using namespace std;

int main(int argc, char** argv){
	Chat s = Chat(8090);
	s.poll();
	return 0;
}