/*
 * Utils.cpp
 *
 *  Created on: Oct 18, 2019
 *      Author: jeremy
 */

#include <vector>
#include <sstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <iomanip>
#include "Downloader.h"
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

void Utils::Process(std::tm start, std::tm end, ThreadSafeMap<std::string, int> & ids)
{
	const std::string baseurl = "http://statsapi.mlb.com";
	std::tm curtm = start;
	while (curtm.tm_mon != end.tm_mon || curtm.tm_mday != end.tm_mday)
	{
		std::stringstream url;
		url.str(std::string());
		url << baseurl << "/api/v1/schedule/games/?sportId=1&date=";
		if (curtm.tm_mon+1 < 10)
		{
			url << "0";
		}
		url << curtm.tm_mon+1 << "/";

		if (curtm.tm_mday < 10)
		{
			url << "0";
		}
		url << curtm.tm_mday << "/" << curtm.tm_year+1900;
		std::cout << url.str() << std::endl;
		std::string jsonstr = Downloader::GetContent(url.str());
		nlohmann::json j = nlohmann::json::parse(jsonstr);
		for (auto& dt : j["dates"].items())
		{
			for (auto& gm: dt.value()["games"].items())
			{
				nlohmann::json::string_t gameType = gm.value()["gameType"];
				nlohmann::json::string_t gameState = gm.value()["status"]["detailedState"];
				if (gameType.compare("R") != 0 || (gameState.compare("Final") != 0 && gameState.compare("Completed Early") != 0))
				{
					continue;
				}
				nlohmann::json::string_t gameid = gm.value()["calendarEventID"];

				std::size_t ix = 0, inner = 0;
				int cnt = 0;
				inner = gameid.find("-");
				while (cnt < 2 && inner != std::string::npos)
				{
					ix = inner;
					++cnt;
					++inner;
					inner = gameid.find("-",inner);
				}

				bool skip = false;
				if (cnt == 2)
				{
					std::string strId = gameid.substr(0,ix);
					std::cout << "strId = " << strId << std::endl;
					if (ids.DoesKeyExist(strId))
					{
						std::cout << "skipping " << gameid << std::endl;
						skip = true;
					}
					else
					{
						ids.set(strId,1);
					}
				}

				if (skip)
				{
					continue;
				}

				nlohmann::json::string_t gamelink = gm.value()["link"];
				std::stringstream gameurl;
				gameurl.str(std::string());
				gameurl << baseurl << gamelink;
				std::string gamejson = Downloader::GetContent(gameurl.str());
				nlohmann::json gamejsonobj = nlohmann::json::parse(gamejson);
				nlohmann::json::string_t altlink = gamejsonobj["link"];
				std::stringstream altgamelink;
				altgamelink.str(std::string());
				altgamelink << baseurl << altlink;
				if (altgamelink.str().compare(gameurl.str()) != 0)
				{
					gamejson = Downloader::GetContent(altgamelink.str());
					gamejsonobj = nlohmann::json::parse(gamejson);
				}
				std::stringstream dirpath;
				dirpath.str(std::string());
				dirpath << "games_json/year_" << curtm.tm_year+1900 << "/month_";
				if (curtm.tm_mon+1 < 10)
			    {
					dirpath << "0";
			    }
				dirpath << curtm.tm_mon+1 << "/day_";
				if (curtm.tm_mday < 10)
				{
					dirpath << "0";
				}
				dirpath << curtm.tm_mday << "/" << gameid << "/";
				if (!std::filesystem::exists(dirpath.str()))
				{
					std::filesystem::create_directories(dirpath.str());
				}
				std::ofstream ofs(dirpath.str()+"game.json");
				ofs << gamejsonobj.dump(4) << std::endl;
				ofs.close();
				std::cout << gameType << "," << gameState << "," << gameid << "," << gameurl.str() << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		++curtm.tm_mday;
		std::mktime(&curtm);
	}
}

