/*
 * main.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: jeremy
 */

#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Downloader.h"
#include "Utils.h"

int main(int argc, char** argv)
{
	std::string usage = "PBPDownloader <start date> <end date (optional)>";
	std::tm startdtm = {0};
	std::tm enddtm = {0};
	if (argc == 3)
	{
		std::string startdate = argv[1];
		std::time_t startts = Utils::GetTimestamp(startdate, &startdtm);
		std::string enddate = argv[2];
		std::time_t endts = Utils::GetTimestamp(enddate, &enddtm);
		if (startts > endts)
		{
			std::cout << "start time is later than end time" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	else if (argc == 2)
	{
		std::string startdate = argv[1];
		std::time_t startts = Utils::GetTimestamp(startdate, &startdtm);
		std::time_t curts = std::time(nullptr);
		enddtm = *std::localtime(&curts);
		if (startts > curts)
		{
			std::cout << "start time is later than end time" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	else
	{
		std::cout << usage << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::string baseurl = "http://statsapi.mlb.com";
	std::tm curtm = startdtm;
	while (curtm.tm_mon != enddtm.tm_mon || curtm.tm_mday != enddtm.tm_mday)
	{
		static std::stringstream url;
		url.seekp(std::ios::beg);
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
				nlohmann::json::string_t gamelink = gm.value()["link"];
				static std::stringstream gameurl;
				gameurl.seekp(std::ios::beg);
				gameurl << baseurl << gamelink;
				std::string gamejson = Downloader::GetContent(gameurl.str());
				nlohmann::json gamejsonobj = nlohmann::json::parse(gamejson);
				nlohmann::json::string_t altlink = gamejsonobj["link"];
				static std::stringstream altgamelink;
				altgamelink.seekp(std::ios::beg);
				altgamelink << baseurl << altlink;
				if (altgamelink.str().compare(gameurl.str()) != 0)
				{
					gamejson = Downloader::GetContent(altgamelink.str());
					gamejsonobj = nlohmann::json::parse(gamejson);
				}
				static std::stringstream dirpath;
				dirpath.seekp(std::ios::beg);
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
				ofs << gamejson << std::endl;
				ofs.close();
				std::cout << gameType << "," << gameState << "," << gameid << "," << gameurl.str() << std::endl;
			}
		}
		++curtm.tm_mday;
		std::mktime(&curtm);
	}
	return 0;
}
