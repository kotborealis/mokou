#pragma once

#include <vector>
#include <string>

using namespace std;

class Chat_History{
	public:
		Chat_History(){};
		void pushMessage(string json);
		vector<string> getMessage(int count);
	private:	
		vector<string> history;
};