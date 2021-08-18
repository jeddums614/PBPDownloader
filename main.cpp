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
#include <cstring>
#include <ctime>
#include "Utils.h"
#include "ThreadSafeMap.h"
#include "ThreadPool.h"

int main(int argc, char** argv)
{
	if (argc > 2)
	{
		std::string usage = "PBPDownloader dates.txt";
		std::cout << usage << std::endl;
		std::exit(-322);
	}

	std::vector<std::pair<std::string, std::optional<std::string>>> dates;
	if (argc == 2) {
		if (!std::filesystem::exists(argv[1]))
		{
			std::cout << argv[1] << " does not exist" << std::endl;
			std::exit(-422);
		}

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
	}
	else {
		std::chrono::system_clock::time_point cur = std::chrono::system_clock::now();
		std::time_t tmptt = std::chrono::system_clock::to_time_t(cur);
		std::tm* ptm = std::localtime(&tmptt);
		--ptm->tm_mday;
		std::mktime(ptm);
		std::string yesterdaystr = ((ptm->tm_mon+1) < 10 ? "0" : "") + std::to_string(ptm->tm_mon+1) + "/" + (ptm->tm_mday < 10 ? "0" : "") + std::to_string(ptm->tm_mday) + "/" + std::to_string(ptm->tm_year+1900);
		dates.push_back(std::make_pair(yesterdaystr, std::nullopt));
	}

	ThreadSafeMap<std::string, int> gameIds;
	unsigned int numThreads = std::thread::hardware_concurrency();
	if (numThreads == 0)
	{
		numThreads = sysconf(_SC_NPROCESSORS_ONLN);
	}

	ThreadPool tp;
	std::vector<std::future<void>> results;

	for (std::size_t i = 0; i < dates.size(); ++i) {
		std::pair<std::string, std::optional<std::string>> dt = dates[i];
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

		results.emplace_back(tp.enqueue([startdtm,enddtm,&gameIds](){Utils::Process(startdtm,enddtm,gameIds);}));
	}

	for (auto& res : results) {
		res.get();
	}

	return 0;
}
