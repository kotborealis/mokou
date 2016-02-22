#include "chat.h"
#include "jsonxx.h"

using namespace std;
using namespace jsonxx;

Chat::Chat(int sp): Websockets(sp){};

void Chat::ws_on_connect(int clientid){
	chat_clients[clientid].name="";
	chat_clients[clientid].loggedIn=false;
	chat_clients[clientid].id=g_id++;
	//ws_broadcast(string("{\"t\":\"user\",\"event\":\"in\",\"user\":{\"name\":\"")+chat_clients[clientid].name+string("\",\"id\":")+to_string(chat_clients[clientid].id)+string("}}"));	
};
void Chat::ws_on_close(int clientid){
	broadcast_loggedOut();
	chat_clients.erase(clientid);
};
void Chat::ws_on_message(int clientid, string message){
	//ws_broadcast(message);	
	CLIENT_ID=clientid;
	cout<<CLIENT_ID<<" - client id\n";
	Object o;
	o.parse(message);
	if(o.has<string>("act"))
		if(o.get<string>("act")=="login" && o.has<string>("name")){
			handler_login(o.get<string>("name"));
		}
		else if(o.get<string>("act")=="logout"){
			handler_logout();
		}
	else
		dispatch_error();
};


//-----------------HANDLERS------------------
void Chat::handler_login(string name){
	encode(name);
	if(name=="" || name.size()>50){
		dispatch_login_error(BAD_NAME);
		return;
	}
	else{
		for(auto it=chat_clients.begin();it!=chat_clients.end();it++){
			cout<<it->second.name<<"=="<<name<<"\n";
			if(it->second.name==name){
				dispatch_login_error(ALREADY_IN);
				cout<<"Nyet!!! (da)";
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
		dispatch_loggedOut();
		broadcast_loggedOut();
	}
}
//------------------END-----------------------

//-----------------DISPATCH ERROR---------------------
void Chat::dispatch_login_error(login_error_type error){
	ws_send(CLIENT_ID,string("{\"result\":\"login_error\",\"id\":\"")+to_string(error)+string("\"}"));
}
void Chat::dispatch_error(){
	ws_send(CLIENT_ID,string("{\"result\":\"error\"}"));
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