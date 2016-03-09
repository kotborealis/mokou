#pragma once

#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>

namespace debug{
	std::string get_time(){
		auto t = std::time(0);
		return "["+std::to_string(t)+"]";
	}

	void i_log(std::string s){
		std::cout<<get_time()<<"[d_log]: "<<s<<"\n";
	}
	void e_log(std::string s){
		std::cout<<get_time()<<"[e_log]: "<<s<<"\n";
	}
}