#include "chat_db.h"
#include <vector>
#include "debug.h"
#include "utils.h"
#include <string>

using namespace std;
using namespace debug;

Chat_db::Chat_db(string ip,int port){
	c = redisConnect(ip.c_str(), port);
	if(c->err){
		e_log("Redis connection error");
		exit(1);
	}
	i_log("Connected");
}
vector<string> Chat_db::getLastMessages(int count){
	vector<string> v;
	reply=(redisReply*)redisCommand(c,"LRANGE mokou:messages 0 %d",count);
	if(reply->type != REDIS_REPLY_ARRAY){
		e_log("Returned non-array object from lrange");
	}
	else{
		v.reserve(reply->elements);
		i_log(to_string(reply->elements));
		for(int i=0;i<reply->elements;i++){
			v.push_back(reply->element[i]->str);
		}
	}
	return v;
}
void Chat_db::pushMessage(string msg){
	i_log(msg);
	reply=(redisReply*)redisCommand(c,"LPUSH mokou:messages %s",msg.c_str());
}