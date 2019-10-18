/*
 * Downloader.h
 *
 *  Created on: Oct 13, 2019
 *      Author: jeremy
 */

#ifndef DOWNLOADER_H_
#define DOWNLOADER_H_

#include <string>

class Downloader
{
public:
	static std::string GetContent(const std::string & url);
	Downloader() = delete;
	Downloader(const Downloader &) = delete;
	Downloader& operator=(const Downloader &) = delete;
	~Downloader() = delete;
};



#endif /* DOWNLOADER_H_ */
