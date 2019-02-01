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
			/**
			 * Deleted default constructor
			 */
			ResponseHeading() = delete;

			/**
			 * Default destructor
			 */
			~ResponseHeading() = default;
			/**
			 * Constructor
			 * @param code HTTP status do you need for your header
			 */
			explicit ResponseHeading(http::StatusCode code) : statusCode{code} {}

			/**
			 * Return the HTTP status code
			 * @return
			 */
			http::StatusCode getStatusCode() const noexcept { return statusCode; }
			/**
			 * Set the HTTP status code
			 * @param code New HTTP code to set
			 */
			void setStatusCode(http::StatusCode const &code) noexcept { statusCode = code; }

			/**
			 * Create a Header with a key and value
			 * @param key for HTTP header
			 * @param value add Value with the key
			 */
			void setHeader(std::string const &key, std::string const &value) {
				if (!headers.count(key)) {
					headers.emplace(key, value);
					return;
				}
				headers[key] += "," + value;
			}

			/**
			 * Return header for the key
			 * @param key Value that you searching
			 * @return the value if it exist
			 */
			std::optional<std::string> getHeader(std::string const &key) const {
				if (headers.count(key))
					return std::optional<std::string>{headers.at(key)};
				return std::nullopt;
			}

			/**
			 * Create a string header
			 * @return The string header when you asked it as a string
			 */
			operator std::string() const {
				std::string response("HTTP/1.1 ");
				response += std::to_string(static_cast<int>(statusCode)) + CRLF;
				for (auto &header : headers)
					response += header.first + ":" + header.second + CRLF;
				response += CRLF;
				return response;
			}

			/**
			 * Overload operator[] to make it work af a operator[] of std::map<std::string, std::string>
			 * @param key for access to HTTP value
			 * @return the header value
			 */
			std::string &operator[](std::string const &key) {
				return headers[key];
			}

			/**
			 * Overload operator[] to make it work af a operator[] of std::map<std::string, std::string>
			 * @param key for access HTTP value
			 * @return the consteness string of header
			 */
			std::string const &operator[](std::string const &key) const {
				return headers.at(key);
			}

	private:
				std::unordered_map<std::string, std::string> headers;
				http::StatusCode statusCode;
		};

}
