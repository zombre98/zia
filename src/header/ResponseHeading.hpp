//
// Created by Thomas Burgaud on 2019-01-22.
//

#pragma once

#include <unordered_map>
#include <string>
#include <numeric>
#include "HeaderEnum.hpp"

#define CRLF "\r\n"

namespace header {

	class ResponseHeading {
		public:
			ResponseHeading() = delete;
			explicit ResponseHeading(http::StatusCode code) : statusCode{code} {};

			http::StatusCode getStatusCode() const noexcept { return statusCode; }
			void setStatusCode(http::StatusCode const &code) noexcept { statusCode = code; }

			void setHeader(std::string const &key, std::string const &value) {
				if (!headers.count(key)) {
					headers.emplace(key, value);
					return;
				}
				headers[key] += "," + value;
			}

			std::optional<std::string> getHeader(std::string const &key) const {
				if (headers.count(key))
					return std::optional<std::string>{headers.at(key)};
				return std::nullopt;
			}

			operator std::string() const {
				std::string response("HTTP/1.1 ");
				response += std::to_string(static_cast<int>(statusCode)) + CRLF;
				response = std::transform_reduce(headers.cbegin(), headers.cend(), response, std::plus<>(), [](auto const &header) {
					return header.first + ":" + header.second + CRLF;
				});
				response += CRLF;
				return response;
			}

			std::string &operator[](std::string const &key) {
				return headers[key];
			}

		std::string const &operator[](std::string const &key) const{
			return headers.at(key);
		}

	private:
				std::unordered_map<std::string, std::string> headers;
				http::StatusCode statusCode;
		};

}
