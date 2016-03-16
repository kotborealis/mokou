#pragma once
#include "chat_client.h"
#include "chat_error.h"

#include <string>

using namespace std;

class Chat_Gen{
	public:
		/**
		 * Create json string (message)
		 */
		static string message(Chat_Client *user, string text, time_t ts);
		/**
		 * Create json string (fl_sys message)
		 */
		static string message(Chat_Client *user, string text, bool fl_sys, time_t ts);
		/**
		 * Create json string (user)
		 */
		static string user(Chat_Client *user, time_t ts);
		/**
		 * Create json string (event)
		 */
		static string event(string type);
		/**
		 * Create json string (erro)
		 */
		static string error(chat_error_type error);
};