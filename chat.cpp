#include "chat.h"

using namespace std;

Chat::Chat(int sp): Websockets(sp){};

void Chat::ws_on_connect(int clientid){
	ws_broadcast("New client.");
};
void Chat::ws_on_close(int clientid){
	ws_broadcast("Client disconnected.");	
};
void Chat::ws_on_message(int clientid, string message){
	ws_broadcast(message);	
};