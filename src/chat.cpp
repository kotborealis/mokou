#include "chat.h"
#include "utils.h"
#include <ctime>
#include <string>
#include "chat_client.h"
#include "jsonxx.h"

using namespace jsonxx;
using namespace std;

Chat::Chat(int sp): Websockets(sp){};

void Chat::ws_on_connect(int clientid){
	CLIENT_ID=clientid;
	delete chat_clients[CLIENT_ID];
	chat_clients[CLIENT_ID] = new Chat_Client(gid++);
	chat_clients[CLIENT_ID]->username="";
	chat_clients[CLIENT_ID]->loggedIn=false;

	for(auto it=chat_clients.begin();it!=chat_clients.end();it++)
			if(it->second->loggedIn)
				ws_send(CLIENT_ID,Chat_Gen::user(it->second,time(0)));

	vector<string> h=chat_db->getMessages(200);
	for(auto it=h.begin();it!=h.end();it++)
		ws_send(CLIENT_ID,*it);
}
void Chat::ws_on_close(int clientid){
	CLIENT_ID=clientid;
	if(chat_clients[CLIENT_ID]->loggedIn)
		handler_logout();
	chat_clients.erase(CLIENT_ID);
}
void Chat::ws_on_message(int clientid, string message){
	CLIENT_ID=clientid;
	Object o;
	o.parse(message);
	if(o.has<string>("act"))
		if(o.get<string>("act")=="login" && o.has<string>("name"))
			handler_login(o.get<string>("name"));
		else if(o.get<string>("act")=="logout")
			handler_logout();
		else if(o.get<string>("act")=="msg" && o.has<string>("msg"))
			handler_message(o.get<string>("msg"));
	else
		ws_send(CLIENT_ID,Chat_Gen::error(UNKNOWN_ERR));
}

void Chat::handler_login(string name){
	utils::encodeHTML(name);
	if(name=="" || utils::Utf8StringLength(name)>50 || utils::Utf8StringLength(name)<3){
		ws_send(CLIENT_ID,Chat_Gen::error(BAD_NAME));
		return;
	}
	else{
		for(auto it=chat_clients.begin();it!=chat_clients.end();it++){
			if(it->second->loggedIn && it->second->username==name){
				ws_send(CLIENT_ID,Chat_Gen::error(ALREADY_IN));
				return;
			}
		}
	}
	chat_clients[CLIENT_ID]->username=name;
	chat_clients[CLIENT_ID]->loggedIn=true;
	ws_send(CLIENT_ID,Chat_Gen::event("loggedIn"));
	ws_broadcast(Chat_Gen::user(chat_clients[CLIENT_ID],time(0)));
	string msg=Chat_Gen::message(chat_clients[CLIENT_ID],"<i>В чат входит <b>"+string(chat_clients[CLIENT_ID]->username)+"</b></i>",true,time(0));
	ws_broadcast(msg);
	chat_db->addMessage(msg);
	return;
}

void Chat::handler_logout(){
	if(chat_clients[CLIENT_ID]->loggedIn){
		chat_clients[CLIENT_ID]->loggedIn=false;
		ws_send(CLIENT_ID,Chat_Gen::event("loggedOut"));
		ws_broadcast(Chat_Gen::user(chat_clients[CLIENT_ID],time(0)));
		string msg=Chat_Gen::message(chat_clients[CLIENT_ID],"<i><b>"+string(chat_clients[CLIENT_ID]->username)+"</b> выходит из чата</i>",true,time(0));
		ws_broadcast(msg);
		chat_db->addMessage(msg);
	}
}

void Chat::handler_message(string message){
	utils::encodeHTML(message);
	if(message=="" || utils::Utf8StringLength(message)>300 || utils::Utf8StringLength(message)==0){
		ws_send(CLIENT_ID,Chat_Gen::error(BAD_MESSAGE));
		return;
	}
	else if(!chat_clients[CLIENT_ID]->loggedIn){
		ws_send(CLIENT_ID,Chat_Gen::error(NOT_LOGGED_IN));
		return;	
	}
	else{
		string msg=Chat_Gen::message(chat_clients[CLIENT_ID],message,time(0));
		ws_broadcast(msg);
		chat_db->addMessage(msg);
	}
}