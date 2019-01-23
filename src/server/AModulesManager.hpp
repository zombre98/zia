#pragma once

#include <string>
#include <map>

namespace api {

struct Context {
	std::string header;
	std::string body;
};


enum class HookTime {
	FIRST,
	MIDDLE,
	LAST
};

class Stage {
	using hookModuleCallback = std::function<void(Context &)>;
	using hookMap = std::map<std::string, hookModuleCallback>;
public:
	void hookToDebut(const std::string &moduleName, hookModuleCallback &&callback) {
		first_[moduleName] = std::move(callback);
	}
	void hookToMiddle(const std::string &moduleName, hookModuleCallback &&callback) {
		middle_[moduleName] = std::move(callback);
	}
	void hookToEnd(const std::string &moduleName, hookModuleCallback &&callback) {
		last_[moduleName] = std::move(callback);
	}

	const hookMap &debutsHooks() { return first_; }
	const hookMap &middlesHooks() { return middle_; }
	const hookMap &endsHooks() { return last_; }

private:
	hookMap first_;
	hookMap middle_;
	hookMap last_;
};

class AModulesManager {
public:
	virtual void loadModules(const std::string &directoryPath) = 0;
	virtual void loadOneModule(const std::string &filePath) = 0;

public:
	Stage &requests() { return request_; }
	Stage &configs() { return config_; }

private:
	Stage request_;
	Stage config_;
};

}
