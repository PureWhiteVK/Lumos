#include "lumos/core/common.h"
#include "lumos/core/color.h"
#include "lumos/core/exception.h"

#include <filesystem>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <stb_image_write.h>

namespace lumos {

std::shared_ptr<spdlog::logger>
SetupLogger(const std::vector<spdlog::sink_ptr> &sinks) {
  auto logger = spdlog::get(LOGGER_NAME);
  if (!logger) {
    if (sinks.size() > 0) {
      logger = std::make_shared<spdlog::logger>(LOGGER_NAME, std::begin(sinks),
                                                std::end(sinks));
      spdlog::register_logger(logger);
    } else {
      // add multi-thread support
      logger = spdlog::stdout_color_mt(LOGGER_NAME);
    }
    spdlog::set_default_logger(logger);
  }
  return logger;
}

namespace fs = std::filesystem;

fs::path GetDataPath(const fs::path& path) {
  static fs::path data_path = fs::u8path(DATA_PATH);
  return data_path / path;
}

fs::path GetDataPathUtf8(std::string_view name) {
  return GetDataPath(std::filesystem::u8path(name));
}
} // namespace lumos