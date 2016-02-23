#pragma once

#include "websockets.h"
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

		typedef enum chat_error_type{
			ALREADY_IN,
			BAD_PASSWORD,
			BAD_NAME,
			BAD_MESSAGE,
			NOT_LOGGED_IN,
			UNKNOWN_ERR,
		} chat_error_type;

		void encode(string&);

		//handlers
		void handler_login(string login);
		void handler_logout();
		void handler_message(string message);

		//dispatch error
		void dispatch_error(chat_error_type error);

		//dispatch
		void dispatch_loggedIn();
		void dispatch_loggedOut();
		void dispatch_online();

		//broadcast
		void broadcast_loggedIn();
		void broadcast_loggedOut();
		void broadcast_message(string message);
	protected:
		void ws_on_connect(int clientid);
		void ws_on_close(int clientid);
		void ws_on_message(int clientid, string message);
};