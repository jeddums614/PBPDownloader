/*
 * Utils.cpp
 *
 *  Created on: Oct 18, 2019
 *      Author: jeremy
 */

#include <vector>
#include <sstream>
#include "Utils.h"

std::time_t Utils::GetTimestamp(const std::string & datestr, std::tm *tmOut)
{
	std::vector<int> dateParts;
	std::string datePartStr;
	std::istringstream ss(datestr);
	while (std::getline(ss,datePartStr,'/'))
	{
		dateParts.push_back(std::stoi(datePartStr));
	}
	tmOut->tm_year = dateParts[2]-1900;
	tmOut->tm_mon = dateParts[0]-1;
	tmOut->tm_mday = dateParts[1];
	std::time_t tsval = std::mktime(tmOut);
	return tsval;
}

