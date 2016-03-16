#pragma once

#include "websockets.h"
#include "chat_db.h"
#include "chat_client.h"
#include "chat_gen.h"
#include "chat_error.h"
#include <map>
#include <string>

using namespace std;

class Chat: public Websockets{
	public:
		Chat(int sp);

		int CLIENT_ID;
		int gid=0;

		map<int,Chat_Client*> chat_clients;
		Chat_db *chat_db = new Chat_db("127.0.0.1",6379);

		void handler_login(string name);
		void handler_logout();
		void handler_message(string message);

		void send(string message);
	protected:
		void ws_on_connect(int clientid);
		void ws_on_close(int clientid);
		void ws_on_message(int clientid, string message);
};