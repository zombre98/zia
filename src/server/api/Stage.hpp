//
// Created by bilel fourati on 2019-01-31.
//

#pragma once

#include <string>
#include <list>
#include <functional>
#include "Heading.hpp"

namespace dems {

/**
 * @enum CodeStatus
 * Enum returned by each Hook Callback Function set in a Stage
 */
enum class CodeStatus {
	OK,
	DECLINED,
	HTTP_ERROR
};

/**
 * The context that is send to each hook callback when a stage is triggered
 */
struct Context {
	header::HTTPMessage request;
	header::HTTPMessage response;
	int socketFd;
};

/**
 * @class Stage
 * Define function that will be called at a certain Moment.
 * The hook will be at the First, middle and end of this Moment.
 * Firts is Before the Moment
 * Middle is during the Moment
 * End is when the Moment ends
 */
class Stage {
public:
	using hookModuleCallback = std::function<CodeStatus(Context &)>;
	using hookList = std::list<std::pair<std::string, hookModuleCallback>>;
public:
	/**
	 * Hook to the start of the stage
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
	 * @return std::list of the modules hooked to the first
	 */
	const hookList &firstsHooks() { return first_; }
	/**
	 * Return the modules hooked to the middle of the stage
	 * @return std::list of the modules hooked to the middles
	 */
	const hookList &middlesHooks() { return middle_; }
	/**
	 * Return the modules hooked to the end of the stage
	 * @return std::list of the modules hooked to the ends
	 */
	const hookList &endsHooks() { return last_; }

private:
	hookList first_;
	hookList middle_;
	hookList last_;
};

/**
 * @class StageManager
 * Define the differents Stage and manage them
 */
class StageManager {
public:

	/**
	 * Get the whole Request Stage
	 * @return Request Stage
	 */
	Stage &request() { return request_; }

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
	Stage &disconnect() { return disconnection_; }

private:
	Stage request_;
	Stage chunks_;
	Stage connection_;
	Stage disconnection_;
};

}
