#include "debug.h"

std::string debug::get_time(){
	auto t = std::time(0);
	return "["+std::to_string(t)+"]";
}

void debug::i_log(std::string s){
	std::cout<<debug::get_time()<<"[i_log]: "<<s<<"\n";
}
void debug::e_log(std::string s){
	std::cout<<debug::get_time()<<"[e_log]: "<<s<<"\n";
}
