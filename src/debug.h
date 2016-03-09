#pragma once

#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>

namespace debug{
	std::string get_time();
	void i_log(std::string s);
	void e_log(std::string s);
}