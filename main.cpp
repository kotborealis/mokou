#include "server.h"
#include "websockets.h"

using namespace std;

int main(int argc, char** argv){
	Websockets s = Websockets(8090);
	s.poll();
	return 0;
}