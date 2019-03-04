#pragma once

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

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
	~DlWrapper() {
#ifdef WIN32
		if (handler_)
			FreeLibrary(handler_);
#else
		if (handler_)
			dlclose(handler_);
#endif
	};

public:
	/**
	 * Get symbol and return it as a pointer to function
	 * @tparam fncType pointer to function type
	 * @param symbol the symbol to load
	 * @return
	 */
	template<typename fncType>
	fncType getSymbol(const std::string &symbol) const {
#ifdef WIN32
		auto ret = reinterpret_cast<fncType>(GetProcAddress(handler_, symbol.c_str()));
		if (ret == nullptr)
			throw std::runtime_error("Cannot get the symbol");
		return ret;
#else
		auto ret = reinterpret_cast<fncType>(dlsym(handler_, symbol.c_str()));
		char *error = dlerror();

		if (error)
			throw std::runtime_error(error);
		return ret;
#endif
	}

	/**
	 * Open and load a file
	 * @param file the path to the file
	 */
	void open(const std::string &file) {
#ifdef WIN32
		if (handler_) {
			if (FreeLibrary(handler_) == 0)
				throw std::runtime_error("error while closing the lib");
			handler_ = nullptr;
		}
		handler_ = LoadLibrary(TEXT(file.c_str()));
		if (handler_ == nullptr)
			throw std::runtime_error("Error while opening the shared lib");
#else
		if (handler_) {
			if (dlclose(handler_) != 0)
				throw std::runtime_error("error while closing the lib");
			handler_ = nullptr;
		}
		handler_ = dlopen(file.c_str(), RTLD_NOW);
		if (handler_ == nullptr)
			throw std::runtime_error("Error while opening the shared lib");
#endif
	}

private:
#ifdef WIN32
	HINSTANCE handler_ = nullptr;
#else
	void *handler_ = nullptr;
#endif
};

}
