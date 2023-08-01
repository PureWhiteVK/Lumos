#include "spdlog/fmt/bundled/format.h"
#include <exception>
#include <iostream>
#include <stb_image.h>
#include <stb_image_write.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <stdexcept>

#define LOG(LOG_TYPE,...) \
  do { \
    spdlog::LOG_TYPE(__VA_ARGS__); \
  } while (false)

#define INFO(...) LOG(info,__VA_ARGS__)
#define WARN(...) LOG(warn,__VA_ARGS__)
#define ERROR(...) LOG(error,__VA_ARGS__)

namespace fs = std::filesystem;

int main() {
  try {
    fs::path data_path{DATA_PATH};
    auto image_path = data_path / "oberbilk.jpg";
    int height,width,channels;
    int desired_channels = 4;
    uint8_t* data = stbi_load(image_path.c_str(),&width,&height,&channels,desired_channels);
    if(!data) {
      throw new std::runtime_error(fmt::format("failed to load image: {}",image_path.filename().string()));
    }
    INFO("image {} loaded",image_path.filename().string());
    INFO("shape: {}x{}x{}",height,width,channels);
    auto output_path = data_path / "test.bmp";
    int ret = stbi_write_bmp( output_path.c_str(), width, height, desired_channels, data);
    if(!ret) {
      throw new std::runtime_error(fmt::format("failed write bmp: {}",output_path.filename().string()));
    }
    stbi_image_free(reinterpret_cast<void*>(data));
  } catch(std::exception& e) {
    ERROR("Exception: {}",e.what());
  }
  return 0;
}