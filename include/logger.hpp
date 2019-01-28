#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>

//TODO: Add level control with default parameter

namespace ilrd
{

inline void Log(std::string s_)
{
	std::cerr << s_ << std::endl;
}

}//namespace ilrd

#endif // log_hpp
