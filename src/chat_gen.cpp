#include "chat_gen.h"
#include "jsonxx.h"
#include <string>

using namespace jsonxx;
using namespace std;
string Chat_Gen::message(Chat_Client *user, string text, time_t ts){
	Object data;
	data<<"t"<<"msg";
	data<<"from"<<user->username;
	data<<"text"<<text;
	data<<"ts"<<to_string(ts);
	return data.json();
}
string Chat_Gen::message(Chat_Client *user, string text, bool fl_sys, time_t ts){
	Object data;
	if(fl_sys)
		data<<"fl"<<"sys";
	data<<"t"<<"msg";
	data<<"from"<<user->username;
	data<<"text"<<text;
	data<<"ts"<<to_string(ts);
	return data.json();
}
string Chat_Gen::user(Chat_Client *user, time_t ts){
	Object data;
	data<<"t"<<"user";
	if(user->loggedIn)
		data<<"event"<<"in";
	else
		data<<"event"<<"out";
	Object user_;
	user_<<"name"<<user->username;
	user_<<"id"<<to_string(user->id);
	data<<"user"<<user_;
	data<<"ts"<<to_string(ts);
	return data.json();
}
string Chat_Gen::event(string type){
	Object data;
	data<<"t"<<type;
	return data.json();
}
string Chat_Gen::error(chat_error_type error){
	Object data;
	data<<"t"<<"err";
	data<<"err"<<to_string(error);
	return data.json();
}