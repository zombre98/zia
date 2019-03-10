//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "LoadPathModule.hpp"
#include "../api/AModulesManager.hpp"
#include "../../Utils/Utils.hpp"
#include "../api/Config.hpp"

static constexpr char MODULE_NAME[] = "LoadPath";

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
std::string registerHooks(dems::StageManager &manager) {
	manager.request().hookToMiddle(2, MODULE_NAME, [](dems::Context &ctx) {
		auto &requestFirstLine = std::get<dems::header::Request>(ctx.request.firstLine);
		auto &root = std::get<std::string>(ctx.config["root"].v);
		auto requestPath = std::get<dems::header::Request>(ctx.request.firstLine).path;

		std::string path = root + requestPath;
		if (requestPath.find('?') != std::string::npos) {
			auto request = zia::utils::split(requestPath, '?');
			if (request.size() > 1)
				path = root + request[0];
		}

		if (std::filesystem::path(path).extension() != ".php")
			return dems::CodeStatus::DECLINED;

		std::string command;
		setenv("GATEWAY_INTERFACE", "CGI/1.1", 0);
		setenv("REQUEST_SCHEME", "http", 0);
		if (requestFirstLine.method == "GET") {
			auto request = zia::utils::split(requestPath, '?');

			setenv("REQUEST_METHOD", "GET", 1);
			setenv("SCRIPT_FILENAME", path.c_str(), 1);
			if (request.size() > 1) {
				setenv("QUERY_STRING", request[1].c_str(), 1);
			}
			command = "php-cgi " + path;
		} else if (requestFirstLine.method == "POST") {
			setenv("REQUEST_METHOD", "POST", 1);
			setenv("SCRIPT_FILENAME", path.c_str(), 1);
			setenv("CONTENT_LENGTH", ctx.request.headers->getHeader("Content-Length").c_str(), 1);
			setenv("CONTENT_TYPE", ctx.request.headers->getHeader("Content-Type").c_str(), 1);
			command = "echo \"" + ctx.request.body + "\" | php-cgi " + path;
			ctx.request.body = "";
		}

		FILE *file = popen(command.c_str(), "r");

		int ch;
		std::string tmp;
		while((ch = fgetc(file)) != EOF) {
			tmp.push_back(ch);
		}
		pclose(file);

		std::istringstream dataStream(tmp);
		std::string line{""};
		while (std::getline(dataStream, line)) {
			if (line == "\r")
				break;

			auto members = zia::utils::split(line, ' ');
			if (!members[0].empty() && !members[1].empty()) {
				members[0].resize(members[0].size() - 1);
				if (!members[0].empty() && !members[1].empty()) {
					members[1].erase(members[1].size() - 1, 1);
					ctx.response.headers->setHeader(members[0], members[1]);
				}
			}
		}
		ctx.response.body = "";
		while (std::getline(dataStream, line)) {
			ctx.response.body.append(line);
		}
		unsetenv("CONTENT_LENGTH");
		unsetenv("CONTENT_TYPE");
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
