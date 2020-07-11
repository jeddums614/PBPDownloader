/*
 * main.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: jeremy
 */

#include <iostream>
#include <string>
#include <ctime>
#include <map>
#include <vector>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <optional>
#include "Utils.h"
#include "ThreadSafeMap.h"

int main(int argc, char** argv)
{
	std::vector<std::pair<std::string, std::optional<std::string>>> dates = {
		std::make_pair("03/20/2019", "03/22/2019"),
	    std::make_pair("03/28/2019", "04/01/2019"),
	    std::make_pair("04/01/2019", "04/08/2019"),
		std::make_pair("04/08/2019", "04/15/2019"),
	    std::make_pair("04/15/2019", "04/22/2019"),
	    std::make_pair("04/22/2019", "04/29/2019"),
		std::make_pair("04/29/2019", "05/01/2019"),
		std::make_pair("05/01/2019", "05/08/2019"),
		std::make_pair("05/08/2019", "05/15/2019"),
		std::make_pair("05/15/2019", "05/22/2019"),
		std::make_pair("05/22/2019", "05/29/2019"),
		std::make_pair("05/29/2019", "06/01/2019"),
	    std::make_pair("06/01/2019", "06/08/2019"),
		std::make_pair("06/08/2019", "06/15/2019"),
		std::make_pair("06/15/2019", "06/22/2019"),
		std::make_pair("06/22/2019", "06/29/2019"),
		std::make_pair("06/29/2019", "07/01/2019"),
	    std::make_pair("07/01/2019", "07/08/2019"),
		std::make_pair("07/11/2019", "07/18/2019"),
		std::make_pair("07/18/2019", "07/25/2019"),
		std::make_pair("07/25/2019", "08/01/2019"),
		std::make_pair("08/01/2019", "08/08/2019"),
		std::make_pair("08/08/2019", "08/15/2019"),
		std::make_pair("08/15/2019", "08/22/2019"),
		std::make_pair("08/22/2019", "08/29/2019"),
		std::make_pair("08/29/2019", "09/01/2019"),
		std::make_pair("09/01/2019", "09/08/2019"),
		std::make_pair("09/08/2019", "09/15/2019"),
		std::make_pair("09/15/2019", "09/22/2019"),
		std::make_pair("09/22/2019", "09/29/2019"),
		std::make_pair("09/29/2019", "09/30/2019")
	};

	ThreadSafeMap<std::string, int> gameIds;
	std::vector<std::thread> threadVec;
	for (auto& dt : dates)
	{
		std::tm startdtm = {0};
		std::tm enddtm = {0};
		Utils::GetTimestamp(std::get<0>(dt), &startdtm);
		if (std::get<1>(dt).has_value())
		{
			Utils::GetTimestamp(std::get<1>(dt).value(), &enddtm);
		}
		else
		{
			std::time_t curts = std::time(nullptr);
			enddtm = *std::localtime(&curts);
		}

		threadVec.push_back(std::thread(Utils::Process,startdtm,enddtm, std::ref(gameIds)));
	}

	// Iterate over the thread vector
	for (std::thread & th : threadVec)
	{
		// If thread Object is Joinable then Join that thread.
		if (th.joinable())
		{
			th.join();
		}
	}

	return 0;
}
