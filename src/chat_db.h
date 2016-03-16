#pragma once

#include "hiredis/hiredis.h"
#include <vector>
#include <string>

using namespace std;

class Chat_db{
	public:
		Chat_db(string ip,int port);
		
		/**
		 * Get lasn N messages from redis
		 * @param {int} count
		 * @returns {vector<string>}
		 */
		vector<string> getMessages(int count);
		/**
		 * Add new message to redis
		 * @param {string} msg JSON object
		 */
		void addMessage(string msg);

		/**
		 * Return vector with online users
		 * @returns {vector<string>}
		 */
		vector<string> getOnline();
		/**
		 * Add user to online list
		 * @param {string} token 
		 * @param {string} data
		 */
		void addOnline(string token, string data);
		/**
		 * Remove user from online list
		 * @param {string} token
		 */
		void removeOnline(string token);
	private:
		redisReply *reply;
		redisContext *c;
};