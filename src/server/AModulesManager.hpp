#pragma once

#include <string>
#include <list>
#include <variant>

namespace api {

/**
 * The context that is send to each hook callback when a stage is triggered
 */

class IHeading {
public:
	virtual ~IHeading() = default;

/*	virtual std::string &getHeader(const std::string &headerName) const = 0;

	virtual std::string &getStatusMessage() const = 0;

	virtual void setHeader(const std::string &key, const std::string &value) = 0;

	virtual void setStatusMessage(const std::string &message) = 0;*/
};

class Heading : public IHeading {
public:
	~Heading() = default;
	Heading() = default;

public:
	std::string str{"Content-Length: 28\r\n\r\n"};
};

struct request {
	std::string method;
	std::string path;
	std::string httpVersion;
};

struct response {
	std::string httpVersion;
	std::string statusCode;
	std::string message;
};

struct HTTPMessage {
	std::variant<request, response> variant;
	std::unique_ptr<IHeading> header;
	std::string body;
};

struct Context {
	HTTPMessage request;
	HTTPMessage response;
	int socketFd;
};

enum class CodeStatus {
	OK,
	DECLINED,
	HTTP_ERROR
};

class Stage {
public:
	using hookModuleCallback = std::function<CodeStatus(Context &)>;
	using hookList = std::list<std::pair<std::string, hookModuleCallback>>;
public:
	/**
	 * Hook to the debut of the stage
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToFirst(const std::string &moduleName, hookModuleCallback &&callback) {
		first_.emplace_back(moduleName, std::move(callback));
	}
	/**
	 * Hook to the middle of the stage
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToMiddle(const std::string &moduleName, hookModuleCallback &&callback) {
		middle_.emplace_back(moduleName, std::move(callback));
	}
	/**
	 * Hook to the end of the stage
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToEnd(const std::string &moduleName, hookModuleCallback &&callback) {
		last_.emplace_back(moduleName, std::move(callback));
	}

	/**
	 * Return the modules hooked to the debut of the stage
	 * @return
	 */
	const hookList &debutsHooks() { return first_; }
	/**
	 * Return the modules hooked to the middle of the stage
	 * @return
	 */
	const hookList &middlesHooks() { return middle_; }
	/**
	 * Return the modules hooked to the end of the stage
	 * @return
	 */
	const hookList &endsHooks() { return last_; }

private:
	hookList first_;
	hookList middle_;
	hookList last_;
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
	 * @return Connection Stage
	 */
	Stage &connection() { return connection_; }

	/**
	 * Get the chunk Stage
	 * @return Chunks Stage
	 */
	Stage &chunks() { return chunks_; }

	/**
	 * Get the disconnect Stage
	 * @return Chunks Stage
	 */
	Stage &disconnect() { return disconnect_; }

private:
	Stage request_;
	Stage chunks_;
	Stage connection_;
	Stage disconnect_;
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
	 * Delete all Modules who have the same name as the parameter
	 * @param name Name os the modules
	 */
	virtual void deleteModule(const std::string &name) = 0;

	/**
	 * Get the Stage Manager
	 * @return Stage Manager
	 */
	StageManager &getStageManager() { return stageManager_; }

private:
	StageManager stageManager_;
};

}
