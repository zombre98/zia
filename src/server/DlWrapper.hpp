#pragma once

#include <dlfcn.h>
#include <string>

namespace zia {

class DlWrapper {
public:
	DlWrapper() = default;

	/**
	 * Construct and load
	 * @param file the path to the file
	 */
	DlWrapper(const std::string &file) {
		open(file);
	}

	/**
	 * Destructor
	 */
	~DlWrapper() { if (handler_) dlclose(handler_); };

public:
	/**
	 * Get symbol and return it as a pointer to function
	 * @tparam fncType pointer to function type
	 * @param symbol the symbol to load
	 * @return
	 */
	template<typename fncType>
	fncType getSymbol(const std::string &symbol) const {
		auto ret = reinterpret_cast<fncType>(dlsym(handler_, symbol.c_str()));
		char *error = dlerror();

		if (error)
			throw std::runtime_error(error);
		return ret;
	}

	/**
	 * Open and load a file
	 * @param file the path to the file
	 */
	void open(const std::string &file) {
		if (handler_) {
			if (dlclose(handler_) != 0)
				throw std::runtime_error("error while closing the lib");
			handler_ = nullptr;
		}
		handler_ = dlopen(file.c_str(), RTLD_NOW);
		if (handler_ == nullptr)
			throw std::runtime_error("Error while opening the shared lib");
	}

private:
	void *handler_ = nullptr;
};

}
