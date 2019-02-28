//
// Created by Thomas Burgaud on 2019-02-23.
//

#pragma once

#include <utility>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <iostream>

namespace zia::utils {
	class JsonParser {

	public:
		explicit JsonParser(std::string filename) : filename_(std::move(filename)) {
			std::cout << "In Json Parser" << std::endl;
			open();
		}

		template<typename T>
		T get(std::string const &key) {
			return data_[key].get<T>();
		}

		bool reloadFile(std::string const &filename) {
			filename_ = filename;
			open();
			return true;
		}

		nlohmann::json &getJsonObject() {
			return data_;
		}

	private:
		void open() {
			std::ifstream fStream(filename_);
			if (!fStream.is_open())
				throw std::runtime_error("Could not open file");
			std::string s((std::istreambuf_iterator<char>(fStream)),
										std::istreambuf_iterator<char>());

			data_ = nlohmann::json::parse(s);
		}

	private:
		std::string filename_;
		nlohmann::json data_;
	};
}
