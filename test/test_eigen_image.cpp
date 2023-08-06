#include "lumos/core/common.h"
#include "lumos/core/color.h"
#include "lumos/core/exception.h"
#include "stb_image_write.h"

namespace fs = std::filesystem;

int main() {
  try {
    constexpr int image_width = 256;
    constexpr int image_height = 256;
    constexpr int image_channels = 4;
    lumos::ImageData4f image(image_height, image_width);
    INFO("image {}x{}x{}", image.rows(), image.cols(), image_channels);
    // (0,0) corresponds to left corner of image
    for (int x = 0; x < image_width; x++) {
      for (int y = 0; y < image_height; y++) {
        image.coeffRef(x, y) = {static_cast<float>(x) / (image_width - 1),
                                static_cast<float>(y) / (image_height - 1),
                                0.0f};
      }
    }

    lumos::ImageData4u8 image_u8 =
        image.unaryExpr([](auto c) { return lumos::toUint8(c); });
    // save png
    INFO("image_u8: {}x{}", image_u8.rows(), image_u8.cols());
    fs::path ouptut_path = lumos::getDataPath("color.png");
    stbi_flip_vertically_on_write(true);
    int res = stbi_write_png(ouptut_path.c_str(), image_u8.cols(),
                             image_u8.rows(), 4, image_u8.data(), 0);
    if (!res) {
      throw lumos::runtime_error("failed to save {}, error code: {}",
                                 ouptut_path, res);
    }
  } catch (std::exception &e) {
    ERROR("Exception: {}", e.what());
  }
}