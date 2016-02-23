#include "chat.h"
#include "jsonxx.h"
#include <ctime>

using namespace std;
using namespace jsonxx;

Chat::Chat(int sp): Websockets(sp){};

void Chat::ws_on_connect(int clientid){
	chat_clients[clientid].name="";
	chat_clients[clientid].loggedIn=false;
	chat_clients[clientid].id=g_id++;
};
void Chat::ws_on_close(int clientid){
	CLIENT_ID=clientid;
	broadcast_loggedOut();
	chat_clients.erase(clientid);
};
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
		dispatch_error(UNKNOWN_ERR);
};


//-----------------HANDLERS------------------
void Chat::handler_login(string name){
	encode(name);
	if(name=="" || name.size()>50){
		dispatch_error(BAD_NAME);
		return;
	}
	else{
		for(auto it=chat_clients.begin();it!=chat_clients.end();it++){
			if(it->second.loggedIn && it->second.name==name){
				dispatch_error(ALREADY_IN);
				return;
			}
		}
	}
	chat_clients[CLIENT_ID].name=name;
	chat_clients[CLIENT_ID].loggedIn=true;
	dispatch_loggedIn();
	broadcast_loggedIn();
	return;
}
void Chat::handler_logout(){
	if(chat_clients[CLIENT_ID].loggedIn){
		chat_clients[CLIENT_ID].loggedIn=false;
		dispatch_loggedOut();
		broadcast_loggedOut();
	}
}
void Chat::handler_message(string message){
	encode(message);
	if(message=="" || message.size()>300){
		dispatch_error(BAD_MESSAGE);
		return;
	}
	else if(!chat_clients[CLIENT_ID].loggedIn){
		dispatch_error(NOT_LOGGED_IN);
		return;	
	}
	else{
		broadcast_message(message);
	}
}
//------------------END-----------------------

//-----------------DISPATCH ERROR---------------------
void Chat::dispatch_error(chat_error_type error){
	ws_send(CLIENT_ID,string("{\"result\":\"error\",\"type\":\"")+to_string(error)+string("\"}"));
}
//---------------------END---------------------

//------------------DISPATCH----------------------------
void Chat::dispatch_loggedIn(){
	ws_send(CLIENT_ID,"{\"t\":\"loggedIn\"}");
}
void Chat::dispatch_loggedOut(){
	ws_send(CLIENT_ID,"{\"t\":\"loggedOut\"}");
}
//---------------------END---------------------------------

//-------------------BROADCAST-----------------
void Chat::broadcast_loggedIn(){
	ws_broadcast(string("{\"t\":\"user\",\"event\":\"in\",\"user\":{\"name\":\"")+chat_clients[CLIENT_ID].name+string("\",\"id\":")+to_string(chat_clients[CLIENT_ID].id)+string("}}"));	
}
void Chat::broadcast_loggedOut(){
	ws_broadcast(string("{\"t\":\"user\",\"event\":\"out\",\"user\":{\"name\":\"")+chat_clients[CLIENT_ID].name+string("\",\"id\":")+to_string(chat_clients[CLIENT_ID].id)+string("}}"));	
}
void Chat::broadcast_message(string message){
	ws_broadcast(string("{\"t\":\"msg\",\"from\":\"")+chat_clients[CLIENT_ID].name+string("\",\"text\":\"")+message+string("\",\"ts\":")+to_string(time(0))+string("}"));
}
//------------------END-------------------

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