//
// Created by Thomas Burgaud on 2019-01-22.
//

#pragma once

#include <memory>
#include <utility>
#include <unordered_map>
#include <string>
#include "server/api/Stage.hpp"
#include "server/api/Heading.hpp"
#include "HeaderEnum.hpp"
#include "Utils/Utils.hpp"
#include "Utils/JsonParser.hpp"

constexpr char CRLF[] = "\r\n";

namespace dems {

namespace header {

inline std::tuple<std::string, std::string, std::string> parseFirstLine(std::string line) {
	std::vector<std::string> tokens = zia::utils::split(std::move(line), ' ');

	if (tokens.size() >= 3)
		return std::make_tuple(tokens[0], tokens[1], tokens[2]);
	return std::make_tuple("", "", "");
}

class Heading : public IHeaders {
		public:
			/**
			 * Default destructor
			 */
			~Heading() = default;

			/**
			 * Default Constructor
			 */
			Heading() = default;

			/**
			 * Overload operator[] to make it work af a operator[] of std::map<std::string, std::string>
			 * @param key for access to HTTP value
			 * @return the header value
			 */
			std::string &operator[](std::string const &key) override {
				return headers_[key];
			}

			/**
			 * Return header for the key
			 * @param key Value that you searching
			 * @return the value if it exist
			 */
			std::string const &getHeader(std::string const &key) const override {
					return headers_.at(key);
			}

			/**
			 * Create a Header with a key and value
			 * @param headerName for HTTP header
			 * @param value add Value with the key
			 */
			void setHeader(std::string const &headerName, std::string const &value) override {
				if (!headers_.count(headerName)) {
					headers_.emplace(headerName, value);
					return;
				}
				headers_[headerName] = value;
			}

			/**
			* Return all the headers separated the line by one CRLF
			* @return All headers add by setHeader of operator[]
			*/
			std::string getWholeHeaders() const override {
				std::string headers;
				for (auto const &it : headers_) {
					headers += it.first + ":" + it.second + CRLF;
				}
				return headers;
			}

			private:
				std::unordered_map<std::string, std::string> headers_;
		};

inline void fillHeading(const std::string &data, dems::Context &context, IHeaders &heading) {
	std::istringstream dataStream(data);

	std::string line;
	std::getline(dataStream, line);
	const auto &[method, path, httpversion] = dems::header::parseFirstLine(line);
	context.request.firstLine = dems::header::Request {method, path, httpversion};

	while (std::getline(dataStream, line)) {
		if (line == "\r")
			break;

		auto members = zia::utils::split(line, ' ');
		if (!members[0].empty() && !members[1].empty()) {
			members[0].resize(members[0].size() - 1);
			if (!members[0].empty() && !members[1].empty()) {
				members[1].erase(members[1].size() - 1, 1);
				heading[members[0]] = members[1];
			}
		}
	}
	while (std::getline(dataStream, line)) {
		context.request.body += line;
	}
}

inline void constructObject(dems::config::Config &config, nlohmann::json const &jsonObject);

inline void createJsonArray(dems::config::ConfigArray &arrConf, nlohmann::json const &jsonObject) {
	for (auto &arrIt : jsonObject) {
		if (arrIt.is_number())
			arrConf.emplace_back(dems::config::ConfigValue{arrIt.get<long long>()});
		else if (arrIt.is_boolean())
			arrConf.emplace_back(dems::config::ConfigValue{arrIt.get<bool>()});
		else if (arrIt.is_string())
			arrConf.emplace_back(dems::config::ConfigValue{arrIt.get<std::string>()});
		else if (arrIt.is_array()) {
			arrConf.emplace_back(dems::config::ConfigValue{dems::config::ConfigArray{}});
			createJsonArray(std::get<dems::config::ConfigArray>(arrConf.back().v), arrIt);
		} else if (arrIt.is_object()) {
			arrConf.emplace_back(dems::config::ConfigValue{dems::config::ConfigObject{}});
			constructObject(std::get<dems::config::ConfigObject>(arrConf.back().v), arrIt);
		}
	}
}

inline void constructObject(dems::config::Config &config, nlohmann::json const &jsonObject) {
	for (nlohmann::json::const_iterator it = jsonObject.begin(); it != jsonObject.end(); ++it) {
		if (jsonObject[it.key()].is_string()) {
			config[it.key()].v = jsonObject[it.key()].get<std::string>();
		}
		if (jsonObject[it.key()].is_boolean()) {
			config[it.key()].v = jsonObject[it.key()].get<bool>();
		}
		if (jsonObject[it.key()].is_number()) {
			config[it.key()].v = jsonObject[it.key()].get<long long>();
		}
		if(jsonObject[it.key()].is_array()) {
			config.emplace(it.key(), dems::config::ConfigValue{dems::config::ConfigArray{}});
			createJsonArray(std::get<dems::config::ConfigArray>(config[it.key()].v), jsonObject[it.key()]);
		}
		if (jsonObject[it.key()].is_object()) {
			config.emplace(it.key(), dems::config::ConfigValue{dems::config::ConfigObject{}});
			constructObject(std::get<dems::config::Config>(config[it.key()].v), jsonObject[it.key()].get<nlohmann::json>());
		}
	}
}

inline void constructConfig(Context &ctx, zia::utils::JsonParser &config) {
	auto &jsonObject = config.getJsonObject();
	ctx.config.clear();
	constructObject(ctx.config, jsonObject);
}

inline void resetContext(Context &ctx) {
	try {
		auto &response = std::get<header::Response>(ctx.response.firstLine);
		auto &request = std::get<header::Request>(ctx.response.firstLine);

		request.method.clear();
		request.httpVersion.clear();
		request.path.clear();
		response.httpVersion.clear();
		response.statusCode.clear();
		response.message.clear();
		ctx.request.headers = std::make_unique<Heading>();
		ctx.response.headers = std::make_unique<Heading>();
		ctx.response.body = "";
		ctx.request.body = "";
	} catch (std::bad_variant_access &e) {
		return;
	}
}

inline std::string constructResponse(Context &context) {
	std::string response;
	header::Response ctxResponse;
	try {
		ctxResponse = std::get<header::Response>(context.response.firstLine);
	} catch (const std::bad_variant_access& e) {
		ctxResponse = header::Response{"HTTP/1.1", "501", "Not_Implemented"};
		context.response.headers->setHeader("Content-Length", "0");
	}

	response = ctxResponse.httpVersion + " " + ctxResponse.statusCode + " " + ctxResponse.message + CRLF;
	response += context.response.headers->getWholeHeaders() + CRLF;
	response += context.response.body;
	resetContext(context);
	return response;
}

}

}

