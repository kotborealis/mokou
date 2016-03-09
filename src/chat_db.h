#pragma once

#include "hiredis/hiredis.h"
#include <vector>
#include <string>

using namespace std;

class Chat_db{
	public:
		Chat_db(string ip,int port);
		vector<string> getLastMessages(int count);
		void pushMessage(string msg);
	private:
		redisReply *reply;
		redisContext *c;
};