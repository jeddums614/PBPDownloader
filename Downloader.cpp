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
		curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,&content);
	    CURLcode res = curl_easy_perform(curl);
	    if (res != CURLE_OK)
	    {
	    	std::cout << "Unable to retrieve webpage " << url << ": " << res << std::endl;
	    	curl_easy_cleanup(curl);
	    	std::exit(EXIT_FAILURE);
	    }
	    curl_easy_cleanup(curl);
	}

	return content;
}
