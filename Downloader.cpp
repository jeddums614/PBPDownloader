/*
 * Downloader.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: jeremy
 */

#include "Downloader.h"
#include <curl/curl.h>
#include <iostream>

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Downloader::GetContent(const std::string & url)
{
	CURL* curl = curl_easy_init();
	std::string content;

	if (curl)
	{
		CURLcode res;
		do
		{
			curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
			curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback);
			curl_easy_setopt(curl,CURLOPT_WRITEDATA,&content);
		    res = curl_easy_perform(curl);
		}
		while (res != CURLE_OK);

	    curl_easy_cleanup(curl);
	}

	return content;
}
