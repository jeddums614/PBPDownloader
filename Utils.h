/*
 * Utils.h
 *
 *  Created on: Oct 18, 2019
 *      Author: jeremy
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <ctime>
#include <string>
#include "ThreadSafeMap.h"

class Utils
{
public:
	Utils() = delete;
	Utils(const Utils &) = delete;
	~Utils() = delete;
	static std::time_t GetTimestamp(const std::string & datestr, std::tm* tmOut);
	static void Process(std::tm start, std::tm end, ThreadSafeMap<std::string, int> & ids);
};



#endif /* UTILS_H_ */
