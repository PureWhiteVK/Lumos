#include <filesystem>
#include <memory>

#include <ImfRgbaFile.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "lumos/core/color.h"
#include "lumos/core/common.h"
#include "lumos/core/imageio.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;

void remove_exist(const fs::path &p) {
  if (fs::exists(p)) {
    DEBUG("path: {} already exists, delete!", p.c_str());
    fs::remove(p);
  }
}

int main() {
  try {
    auto logger = lumos::setupLogger(
        {std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
    spdlog::set_level(spdlog::level::debug);
    fs::path exr_path = lumos::getDataPath("dragon-ao.exr");
    lumos::ImageData4f exr_image = lumos::readExr(exr_path);
    DEBUG("exr_image(hxw): {}x{}", exr_image.rows(), exr_image.cols());

    lumos::ImageData4u8 png_image = exr_image.unaryExpr(
        [](auto c) { return lumos::toUint8(lumos::toSrgb(c)); });
    lumos::savePng(lumos::getDataPath("dragon-ao.png"), png_image);
    lumos::ImageData4h pic_4h = exr_image.unaryExpr(
        [](const lumos::Color4f &c) { return lumos::toImfRgba(c); });

    fs::path png_path_output = lumos::getDataPath("dragon-ao-block.png");
    fs::path exr_path_output = lumos::getDataPath("dragon-ao-block.exr");
    remove_exist(png_path_output);
    remove_exist(exr_path_output);
    {
      int br = 200, bc = 300, bh = 108 * 2, bw = 192 * 2;
      lumos::saveExr(exr_path_output.c_str(), exr_image.rows(),
                     exr_image.cols(), 200, 300,
                     exr_image.block(200, 300, bh, bw));
      lumos::savePng(png_path_output,
                     exr_image.block(200, 300, bh, bw).unaryExpr([](auto c) {
                       return lumos::toUint8(lumos::toSrgb(c));
                     }));
    }

    lumos::readExr(exr_path_output);
    lumos::readPng(png_path_output);
  } catch (const std::exception &e) {
    ERROR(fmt::format("Exception: {}", e.what()));
    return 1;
  }
  return 0;
}