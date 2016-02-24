#include "chat.h"
#include "chat_history.h"
#include "jsonxx.h"
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

	vector<string> h=chat_history.getMessage(150);
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
	encode(name);
	if(name=="" || name.size()>50 || name.size()<3){
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
	chat_history.pushMessage(msg);
	return;
}

void Chat::handler_logout(){
	if(chat_clients[CLIENT_ID].loggedIn){
		chat_clients[CLIENT_ID].loggedIn=false;
		ws_send(CLIENT_ID,json_event("loggedOut"));
		ws_broadcast(json_event_user("out",chat_clients[CLIENT_ID].name,chat_clients[CLIENT_ID].id,time(0)));
		string msg=json_event_user_msg("out",chat_clients[CLIENT_ID].name,time(0));
		ws_broadcast(msg);
		chat_history.pushMessage(msg);
	}
}

void Chat::handler_message(string message){
	encode(message);
	if(message=="" || message.size()>300 || message.size()==0){
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
		chat_history.pushMessage(msg);
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


void Chat::encode(std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    data.swap(buffer);
}

/*
-->
	act: login
	name: string
<--
	result: fail
	[reason: string]
	-------
	t:loggedIn
*/

/*
-->
	act: logout
<--

*/

/*
-->
	act: msg
	msg: string
*/

/*
<--
	t: msg
	from: string
	text: string
	ts: timestamp
*/

/*
<--
	t:user
	event:in/out
	user:{
		name:""
		id:int
	}
*/
/*
<--
	act:logout
-->
	t:loggedOut
*/