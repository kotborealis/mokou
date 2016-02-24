#pragma once

#include "websockets.h"
#include "chat_history.h"
#include <map>

class Chat: public Websockets{
	public:
		Chat(int sp);
	private:
		int g_id=0;
		int CLIENT_ID;

		typedef struct chat_client{
			string name;
			bool loggedIn;
			int id;
		} chat_client;
		map<int,chat_client> chat_clients;

		Chat_History chat_history = Chat_History();

		typedef enum chat_error_type{
			ALREADY_IN,
			BAD_PASSWORD,
			BAD_NAME,
			BAD_MESSAGE,
			NOT_LOGGED_IN,
			UNKNOWN_ERR,
		} chat_error_type;

		void encode(string&);

		string json_message(string from, string text, time_t ts);
		string json_event_user(string event, string name, int user_id, time_t ts);
		string json_event_user_msg(string event, string name, time_t ts);
		string json_event(string type);
		string json_error(chat_error_type error);

		void handler_login(string name);
		void handler_logout();
		void handler_message(string message);
	protected:
		void ws_on_connect(int clientid);
		void ws_on_close(int clientid);
		void ws_on_message(int clientid, string message);
};