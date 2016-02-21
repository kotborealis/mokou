#pragma once

#include "websockets.h"
#include <map>

class Chat: public Websockets{
	public:
		Chat(int sp);
	private:
		typedef struct chat_client{
			string nick;
			//...
		} chat_client;

		map<int,chat_client> chat_clients;
	protected:
		void ws_on_connect(int clientid);
		void ws_on_close(int clientid);
		void ws_on_message(int clientid, string message);
};