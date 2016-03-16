#pragma once

#include <string>

class Chat_Client{
	public:
		Chat_Client(){
			id=0;
		}
		Chat_Client(int id_){
			id=id_;
		}
		int id;
		std::string username="";
		bool loggedIn=false;
};