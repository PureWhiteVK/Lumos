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
    auto logger = lumos::SetupLogger(
        {std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
    spdlog::set_level(spdlog::level::debug);
    fs::path exr_path = lumos::GetDataPath("dragon-ao.exr");
    lumos::ImageData4f exr_image;
    lumos::ReadExr<lumos::ImageData4f>(exr_path, &exr_image);
    DEBUG("exr_image(hxw): {}x{}", exr_image.rows(), exr_image.cols());

    lumos::ImageData4u8 png_image = exr_image.unaryExpr(
        [](auto c) { return lumos::ToUint8(lumos::ToSrgb(c)); });
    lumos::SavePng(lumos::GetDataPath("dragon-ao.png"), png_image);
    lumos::ImageData4h pic_4h = exr_image.unaryExpr(
        [](const lumos::Color4f &c) { return lumos::ToImfRgba(c); });

    fs::path png_path_output = lumos::GetDataPath("dragon-ao-block.png");
    fs::path exr_path_output = lumos::GetDataPath("dragon-ao-block.exr");
    remove_exist(png_path_output);
    remove_exist(exr_path_output);
    {
      int br = 200, bc = 300, bh = 108 * 2, bw = 192 * 2;
      lumos::SaveExr(exr_path_output.c_str(), exr_image.rows(),
                     exr_image.cols(), 200, 300,
                     exr_image.block(200, 300, bh, bw));
      lumos::SavePng(png_path_output,
                     exr_image.block(200, 300, bh, bw).unaryExpr([](auto c) {
                       return lumos::ToUint8(lumos::ToSrgb(c));
                     }));
    }
    lumos::ReadExr<lumos::ImageData4h>(exr_path_output,nullptr);
    lumos::ReadPng(png_path_output,nullptr);
  } catch (const std::exception &e) {
    ERROR(fmt::format("Exception: {}", e.what()));
    return 1;
  }
  return 0;
}