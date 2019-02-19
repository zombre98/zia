//
// Created by Thomas Burgaud on 2019-01-22.
//

#pragma once

#include <unordered_map>
#include <string>
#include "server/api/Heading.hpp"
#include "HeaderEnum.hpp"

constexpr char CRLF[] = "\r\n";

namespace dems {

namespace header {

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
				return headers[key];
			}

			/**
			 * Return header for the key
			 * @param key Value that you searching
			 * @return the value if it exist
			 */
			std::string const &getHeader(std::string const &key) const override {
					return headers.at(key);
			}

			/**
			 * Create a Header with a key and value
			 * @param headerName for HTTP header
			 * @param value add Value with the key
			 */
			void setHeader(std::string const &headerName, std::string const &value) override {
				if (!headers.count(headerName)) {
					headers.emplace(headerName, value);
					return;
				}
				headers[headerName] += "," + value;
			}

			std::string getWholeHeader() const override {
				return "";
			}

			private:
				std::unordered_map<std::string, std::string> headers;
		};
	}
}
