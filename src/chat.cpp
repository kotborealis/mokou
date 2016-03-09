#include "chat.h"
#include "chat_db.h"
#include "jsonxx.h"
#include "utils.h"
#include <ctime>

using namespace std;
using namespace jsonxx;

Chat::Chat(int sp): Websockets(sp){
	
};

void Chat::ws_on_connect(int clientid){
	CLIENT_ID=clientid;
	chat_clients[CLIENT_ID].name="";
	chat_clients[CLIENT_ID].loggedIn=false;
	chat_clients[CLIENT_ID].id=g_id++;

	for(auto it=chat_clients.begin();it!=chat_clients.end();it++)
			if(it->second.loggedIn)
				ws_send(CLIENT_ID,json_event_user("in",it->second.name,it->second.id,time(0)));

	vector<string> h=chat_db->getLastMessages(200);
	for(auto it=h.begin();it!=h.end();it++)
		ws_send(CLIENT_ID,*it);
}
void Chat::ws_on_close(int clientid){
	CLIENT_ID=clientid;
	if(chat_clients[CLIENT_ID].loggedIn)
		handler_logout();
	chat_clients.erase(clientid);
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
		ws_send(CLIENT_ID,json_error(UNKNOWN_ERR));
}

void Chat::handler_login(string name){
	utils::encodeHTML(name);
	if(name=="" || utils::Utf8StringLength(name)>50 || utils::Utf8StringLength(name)<3){
		ws_send(CLIENT_ID,json_error(BAD_NAME));
		return;
	}
	else{
		for(auto it=chat_clients.begin();it!=chat_clients.end();it++){
			if(it->second.loggedIn && it->second.name==name){
				ws_send(CLIENT_ID,json_error(ALREADY_IN));
				return;
			}
		}
	}
	chat_clients[CLIENT_ID].name=name;
	chat_clients[CLIENT_ID].loggedIn=true;
	ws_send(CLIENT_ID,json_event("loggedIn"));
	ws_broadcast(json_event_user("in",chat_clients[CLIENT_ID].name,chat_clients[CLIENT_ID].id,time(0)));
	string msg=json_event_user_msg("in",chat_clients[CLIENT_ID].name,time(0));
	ws_broadcast(msg);
	chat_db->pushMessage(msg);
	return;
}

void Chat::handler_logout(){
	if(chat_clients[CLIENT_ID].loggedIn){
		chat_clients[CLIENT_ID].loggedIn=false;
		ws_send(CLIENT_ID,json_event("loggedOut"));
		ws_broadcast(json_event_user("out",chat_clients[CLIENT_ID].name,chat_clients[CLIENT_ID].id,time(0)));
		string msg=json_event_user_msg("out",chat_clients[CLIENT_ID].name,time(0));
		ws_broadcast(msg);
		chat_db->pushMessage(msg);
	}
}

void Chat::handler_message(string message){
	utils::encodeHTML(message);
	if(message=="" || utils::Utf8StringLength(message)>300 || utils::Utf8StringLength(message)==0){
		ws_send(CLIENT_ID,json_error(BAD_MESSAGE));
		return;
	}
	else if(!chat_clients[CLIENT_ID].loggedIn){
		ws_send(CLIENT_ID,json_error(NOT_LOGGED_IN));
		return;	
	}
	else{
		string msg=json_message(chat_clients[CLIENT_ID].name,message,time(0));
		ws_broadcast(msg);
		chat_db->pushMessage(msg);
	}
}

string Chat::json_message(string from, string text, time_t ts){
	Object data;
	data<<"t"<<"msg";
	data<<"from"<<from;
	data<<"text"<<text;
	data<<"ts"<<to_string(ts);
	return data.json();
}

string Chat::json_event_user(string event, string name, int user_id, time_t ts){
	Object data;
	data<<"t"<<"user";
	data<<"event"<<event;
	Object user;
	user<<"name"<<name;
	user<<"id"<<to_string(user_id);
	data<<"user"<<user;
	data<<"ts"<<to_string(ts);
	return data.json();
}

string Chat::json_event_user_msg(string event, string name, time_t ts){
	Object data;
	data<<"t"<<"msg";
	data<<"from"<<"";
	if(event=="in")
		data<<"text"<<"В чат входит <b>"+string(name)+"</b>";
	else
		data<<"text"<<"<b>"+string(name)+"</b> выходит из чата";
	data<<"ts"<<to_string(ts);
	return data.json();	
}

string Chat::json_event(string type){
	Object data;
	data<<"t"<<type;
	return data.json();
}

string Chat::json_error(chat_error_type error){
	Object data;
	data<<"t"<<"err";
	data<<"err"<<to_string(error);
	return data.json();
}