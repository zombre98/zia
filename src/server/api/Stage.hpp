//
// Created by bilel fourati on 2019-01-31.
//

#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include "Heading.hpp"

/**
 * @brief dems namespace
 */
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
 * The context that is sent to each hook callback when a stage is triggered
 */
struct Context {
	std::vector<uint8_t> rawData;
	header::HTTPMessage request;
	header::HTTPMessage response;
	int socketFd;
};

/**
 * @class Stage
 * Defines function that will be called at a certain Stage.
 * The hook will be at the First, middle and end of this Stage.
 * First is at Stage start
 * Middle is during the Stage
 * End is at Stage end
 */
class Stage {
public:
	/**
	 * @brief Defines a callback function
	 */
	using hookModuleCallback = std::function<CodeStatus(Context &)>;
	/**
	 * @brief Defines a hooked module function
	 */
	struct hook {
		hook(std::string const &name, hookModuleCallback &&function) : moduleName(name), callback(std::move(callback)) {}
		std::string moduleName;
		hookModuleCallback callback;
	};
	/**
	 * @brief Defines a map of hooked functions
	 */
	using hookMap = std::map<uint, hook>;
public:
	/**
	 * Hook to the start of the stage
	 * @param index The index for the map order
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToFirst(uint index, const std::string &moduleName, hookModuleCallback &&callback) {
		first_.emplace(index, hook{moduleName, std::move(callback)});
	}
	/**
	 * Hook to the middle of the stage
	 * @param index The index for the map order
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToMiddle(uint index, const std::string &moduleName, hookModuleCallback &&callback) {
		middle_.emplace(index, hook{moduleName, std::move(callback)});
	}
	/**
	 * Hook to the end of the stage
	 * @param index The index for the map order
	 * @param moduleName The name of the module
	 * @param callback The callback called when a stage Time is triggered
	 */
	void hookToEnd(uint index, const std::string &moduleName, hookModuleCallback &&callback) {
		last_.emplace(index, hook{moduleName, std::move(callback)});
	}

	/**
	 * Returns the modules hooked to the debut of the stage
	 * @return std::list of the modules hooked to the first
	 */
	const hookMap &firstHooks() { return first_; }
	/**
	 * Returns the modules hooked to the middle of the stage
	 * @return std::list of the modules hooked to the middle
	 */
	const hookMap &middleHooks() { return middle_; }
	/**
	 * Returns the modules hooked to the end of the stage
	 * @return std::list of the modules hooked to the end
	 */
	const hookMap &endHooks() { return last_; }

private:
	hookMap first_;
	hookMap middle_;
	hookMap last_;
};

/**
 * @class StageManager
 * Defines the different Stages and manages them
 */
class StageManager {
public:

	/**
	 * Gets the whole Request Stage
	 * @return Request Stage
	 */
	Stage &request() { return request_; }

	/**
	 * Gets the whole Config Stage
	 * @return Config Stage
	 */
	Stage &connection() { return connection_; }

	/**
	 * Gets the chunks Stage
	 * @return Chunks Stage
	 */
	Stage &chunks() { return chunks_; }

	/**
	 * Gets the disconnect Stage
	 * @return Disconnect Stage
	 */
	Stage &disconnect() { return disconnection_; }

private:
	Stage request_;
	Stage chunks_;
	Stage connection_;
	Stage disconnection_;
};

}
