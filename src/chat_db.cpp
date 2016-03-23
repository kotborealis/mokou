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
vector<string> Chat_db::getMessages(int count){
	vector<string> v;
	reply=(redisReply*)redisCommand(c,"LRANGE mokou:messages %d -1",-1*count);
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
void Chat_db::addMessage(string msg){
	i_log(msg);
	reply=(redisReply*)redisCommand(c,"RPUSH mokou:messages %s",msg.c_str());
}

/*vector<string> Chat_db::getOnline(){
	vector<string> v;
	reply=(redisReply*)redisCommand(c,"HGETALL mokou:online");
	if(reply->type != REDIS_REPLY_ARRAY){
		e_log("Returned non-array object from hgetall");
	}
	else{
		v.reserve(reply->elements);
		i_log(to_string(reply->elements));
		for(int i=0;i<reply->elements;i+=2){
			i_log(reply->element[i]->str);
			v.push_back(reply->element[i]->str);
		}
	}
	return v;
}

void Chat_db::addOnline(string token, string data){
	reply=(redisReply*)redisCommand(c,"HSET mokou:online %s %s",token,data);
}

void Chat_db::removeOnline(string token){
	reply=(redisReply*)redisCommand(c,"HDEL mokou:online %s",token);
}*/