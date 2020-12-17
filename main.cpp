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
#include <unistd.h>
#include "Utils.h"
#include "ThreadSafeMap.h"

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::string usage = "PBPDownloader dates.txt";
		std::cout << usage << std::endl;
		std::exit(-322);
	}

	if (!std::filesystem::exists(argv[1]))
	{
		std::string errormsg = "dates.txt does not exist";
		std::cout << errormsg << std::endl;
		std::exit(-422);
	}

	std::vector<std::pair<std::string, std::optional<std::string>>> dates;
	std::ifstream ifs(argv[1]);
	std::string line;
	while (std::getline(ifs, line))
	{
		std::stringstream ss{line};
		std::string ssline, strstartdt = "", strenddt = "";
		while (std::getline(ss, ssline, ' '))
		{
			if (strstartdt.empty())
			{
				strstartdt = ssline;
			}
			else if (strenddt.empty())
			{
				strenddt = ssline;
			}
		}

		if (!strenddt.empty())
		{
			dates.push_back(std::make_pair(strstartdt, strenddt));
		}
		else
		{
			dates.push_back(std::make_pair(strstartdt, std::nullopt));
		}
	}
	ifs.close();

	ThreadSafeMap<std::string, int> gameIds;
	unsigned int numThreads = std::thread::hardware_concurrency();
	if (numThreads == 0)
	{
		numThreads = sysconf(_SC_NPROCESSORS_ONLN);
	}

	for (std::size_t i = 0; i < dates.size(); i+=numThreads)
	{
		std::vector<std::thread> threadVec;
		if ((dates.size()-i) >= numThreads)
		{
			for (std::size_t t = 0; t < numThreads; ++t)
			{
				std::pair<std::string, std::optional<std::string>> dt = dates[i+t];
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
		}
		else
		{
			for (std::size_t j = 0; j < (dates.size()-i); ++j)
			{
				std::pair<std::string, std::optional<std::string>> dt = dates[i+j];
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
	}

	return 0;
}
