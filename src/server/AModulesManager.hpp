#pragma once

#include <string>
#include <map>

namespace api {

/**
 * The context that is send to each hook callback when a stage is triggered
 */

class IHeading {
public:
	virtual ~IHeading() = default;

	virtual std::string &getURL() const = 0;

	virtual uint16_t &getStatusCode() const = 0;

	virtual std::string &getHeader(const std::string &headerName) const = 0;

	virtual std::string &getStatusMessage() const = 0;

	virtual void setURL(const std::string &url) = 0;

	virtual void setStatusCode(const uint16_t &code) = 0;

	virtual void setHeader(const std::string &key, const std::string &value) = 0;

	virtual void setStatusMessage(const std::string &message) = 0;
};

struct HTTPMessage {
	std::unique_ptr<IHeading> header;
	std::string body;
};

struct Context {
	HTTPMessage request;
	HTTPMessage response;
	int socketFd;
};

class Stage {
public:
	using hookModuleCallback = std::function<void(Context &)>;
	using hookMap = std::map<std::string, hookModuleCallback>; // Alphabetical Ordered
public:
	/**
	 * Hook to the debut of the stage
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToDebut(const std::string &moduleName, hookModuleCallback &&callback) {
		first_[moduleName] = std::move(callback);
	}
	/**
	 * Hook to the middle of the stage
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToMiddle(const std::string &moduleName, hookModuleCallback &&callback) {
		middle_[moduleName] = std::move(callback);
	}
	/**
	 * Hook to the end of the stage
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToEnd(const std::string &moduleName, hookModuleCallback &&callback) {
		last_[moduleName] = std::move(callback);
	}

	/**
	 * Return the modules hooked to the debut of the stage
	 * @return
	 */
	const hookMap &debutsHooks() { return first_; }
	/**
	 * Return the modules hooked to the middle of the stage
	 * @return
	 */
	const hookMap &middlesHooks() { return middle_; }
	/**
	 * Return the modules hooked to the end of the stage
	 * @return
	 */
	const hookMap &endsHooks() { return last_; }

private:
	hookMap first_;
	hookMap middle_;
	hookMap last_;
};

class StageManager {
public:
	/**
	 * Get the whole Request Stage
	 * @return Request Stage
	 */
	Stage &requests() { return request_; }
	/**
	 * Get the whole Configs Stage
	 * @return Config Stage
	 */
	Stage &configs() { return config_; }

	/**
	 * Get the whole chunk module
	 * @return Chunks Stage
	 */
	Stage &chunks() { return chunks_; }

private:
	Stage request_;
	Stage config_;
	Stage chunks_;
};

class AModulesManager {
public:
	/**
	 * Load all the modules present in a directory
	 * @param directoryPath the path to the directory containing the modules
	 */
	virtual void loadModules(const std::string &directoryPath) = 0;
	/**
	 * Load one module
	 * @param filePath the path to the module
	 */
	virtual void loadOneModule(const std::string &filePath) = 0;

	/**
	 * Get the Stage Manager
	 * @return
	 */
	StageManager &getStageManager() { return stageManager_; }

private:
	StageManager stageManager_;
};

}
