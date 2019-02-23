//
// Created by bilel fourati on 2019-02-23.
//

#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace zia::utils {

std::vector<std::string> split(std::string str, char delim) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);

	while (std::getline(tokenStream, token, delim))
	{
		tokens.push_back(token);
	}
	return tokens;
}

}
