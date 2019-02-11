#include <iostream>

#include "../api/AModulesManager.hpp"
#include "../../Utils/Logger.hpp"

static constexpr char MODULE_NAME[] = "Logger";

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
void registerHooks(dems::StageManager &manager) {
  manager.request().hookToFirst(1,MODULE_NAME, [](dems::Context &ctx) {
      logging::debug << LOG_DEBUG << "Stage: Request FIRST" << std::endl;
      //logging::debug << LOG_DEBUG << ctx.response.body << std::endl;
      return dems::CodeStatus::OK;
      });

  manager.request().hookToMiddle(1, MODULE_NAME, [](dems::Context &ctx) {
      logging::debug << LOG_DEBUG << "Stage: Request MIDDLE" << std::endl;
      //logging::debug << LOG_DEBUG << ctx.response.body << std::endl;
      return dems::CodeStatus::OK;
      });

  manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
      logging::debug << LOG_DEBUG << "Stage: Request END";
      //logging::debug << LOG_DEBUG << ctx.response.body << std::endl;
      return dems::CodeStatus::OK;
      });
}

};
