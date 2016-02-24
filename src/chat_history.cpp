#include "chat_history.h"
#include <string>
#include <iostream>

using namespace std;

void Chat_History::pushMessage(string message){
	history.push_back(message);
}

vector<string> Chat_History::getMessage(int count){
	vector<string> get;
	for(int i=(count<history.size())?(history.size()-count):0;i<history.size();i++){
		get.push_back(history[i]);
	}
	for(;get.size()>200;)
		get.erase(get.begin());
	return get;
}