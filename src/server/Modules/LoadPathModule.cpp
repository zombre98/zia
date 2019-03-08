//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <filesystem>
#include <unordered_map>
#include "LoadPathModule.hpp"
#include "../api/AModulesManager.hpp"
#include "../../Utils/Logger.hpp"
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
	manager.request().hookToFirst(1, MODULE_NAME, [](dems::Context &ctx) {
		auto &root = std::get<std::string>(ctx.config["root"].v);
		auto path = root + std::get<dems::header::Request>(ctx.request.firstLine).path;
		if (std::filesystem::path(path).extension() == ".php")
			return dems::CodeStatus::DECLINED;

		if (std::filesystem::is_directory(path)) {
		  auto &defaultFile = std::get<std::string>(ctx.config["default_file"].v);
		  path = root + defaultFile;
		}
    std::ifstream fStream(path);
//		if (!fStream.is_open()) {
//			std::cout << "Open failed" << std::endl;
//			ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "404", ""};
//			return dems::CodeStatus::OK;
//		}
		std::string s((std::istreambuf_iterator<char>(fStream)), std::istreambuf_iterator<char>());
		ctx.response.body = std::move(s);

		auto ext = std::filesystem::path(path).extension();
		if (fileExtension.count(ext))
		  ctx.response.headers->setHeader("content-type", fileExtension.at(ext));
		return dems::CodeStatus::OK;
	});

	manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
	//	try {
	//		auto &response = std::get<dems::header::Response>(ctx.request.firstLine);
//		}
//		catch (std::bad_variant_access &e) {
			ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "200", ""};
			ctx.response.headers->setHeader("Content-Length", std::to_string(ctx.response.body.length()));
			return dems::CodeStatus::OK;
	//	}
//		ctx.response.headers->setHeader("Content-Length", std::to_string(ctx.response.body.length()));
//		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
