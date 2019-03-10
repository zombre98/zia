//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <filesystem>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "LoadPathModule.hpp"
#include "../api/AModulesManager.hpp"
#include "../../Utils/Utils.hpp"
#include "../api/Config.hpp"

static constexpr char MODULE_NAME[] = "LoadPath";

static std::unordered_map<std::string, std::string> const fileExtension {
	{".html", "text/html"},
	{".jpeg", "image/jpeg"},
	{".png", "image/png"},
	{".mp4", "video/mp4"},
	{".mp3", "audio/mpeg"},
	{".json", "application/json"},
	{".css", "text/css"}
};

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
std::string registerHooks(dems::StageManager &manager) {
	manager.request().hookToMiddle(1, MODULE_NAME, [](dems::Context &ctx) {
		auto &requestFirstLine = std::get<dems::header::Request>(ctx.request.firstLine);
		auto &root = std::get<std::string>(ctx.config["root"].v);
		auto requestPath = std::get<dems::header::Request>(ctx.request.firstLine).path;

		std::string path = root + requestPath;
		if (requestPath.find('?') != std::string::npos) {
			auto request = zia::utils::split(requestPath, '?');
			if (request.size() > 1)
				path = root + request[0];
		}
		if (std::filesystem::path(path).extension() == ".php")
			return dems::CodeStatus::DECLINED;

		if (std::filesystem::is_directory(path)) {
		  auto &defaultFile = std::get<std::string>(ctx.config["default_file"].v);
		  path = root + defaultFile;
		}
    std::ifstream fStream(path);
		if (!fStream.is_open())
			return dems::CodeStatus::DECLINED;
		std::string s((std::istreambuf_iterator<char>(fStream)), std::istreambuf_iterator<char>());
		ctx.response.body = std::move(s);

		auto ext = std::filesystem::path(path).extension();
		if (fileExtension.count(ext))
		  ctx.response.headers->setHeader("Content-type", fileExtension.at(ext));
		return dems::CodeStatus::OK;
	});

	manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
		auto &root = std::get<std::string>(ctx.config["root"].v);
		auto requestPath = std::get<dems::header::Request>(ctx.request.firstLine).path;

		auto path = root + requestPath;
		if (requestPath.find('?') != std::string::npos) {
			auto request = zia::utils::split(requestPath, '?');
			if (request.size() > 1)
				path = root + request[0];
		}

		if (std::filesystem::is_directory(path)) {
			auto &defaultFile = std::get<std::string>(ctx.config["default_file"].v);
			path = root + defaultFile;
		}
		if (!std::filesystem::exists(path)) {
			ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "404", "Not Found"};
			ctx.response.headers->setHeader("Content-Length", "0");
			return dems::CodeStatus::OK;
		}
		ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "200", ""};
		ctx.response.headers->setHeader("Content-Length", std::to_string(ctx.response.body.length()));
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
