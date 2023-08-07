#include "lumos/core/imageio.h"
#include "lumos/core/color.h"
#include "lumos/core/common.h"
#include "lumos/core/exception.h"

#include <ImfRgba.h>
#include <ImfRgbaFile.h>
#include <arm_neon.h>
#include <filesystem>
#include <stb_image.h>
#include <stb_image_write.h>
#include <type_traits>

namespace lumos {
void readPng(const std::filesystem::path &input, ImageData4u8 *output) {
  DEBUG("read png file: {}", input);
  int height, width, channels;
  std::unique_ptr<uint8_t, std::function<void(void *)>> data_handle(
      stbi_load(input.c_str(), &width, &height, &channels, 4),
      [](void *d) { stbi_image_free(d); });
  if (!data_handle) {
    throw RuntimeError("failed to read png: {}", input);
  }
  if (!output) {
    return;
  }
  *output = ImageDataView4u8(
      reinterpret_cast<ImageDataView4u8::PointerArgType>(data_handle.get()),
      height, width);
}

// ImageData4f readExr(const std::filesystem::path &input) {
//   DEBUG("read exr file: {}", input);
//   Imf::RgbaInputFile file(input.c_str());
//   // support for Cropped image reading and writing
//   Imath::Box2i dw = file.dataWindow();
//   int width = dw.max.x - dw.min.x + 1;
//   int height = dw.max.y - dw.min.y + 1;
//   lumos::ImageData4h pixels(height, width);
//   file.setFrameBuffer(pixels.data() - dw.min.x - dw.min.y * width, 1, width);
//   file.readPixels(dw.min.y, dw.max.y);
//   lumos::ImageData4f res =
//       pixels.unaryExpr([](const Imf::Rgba &c) { return toColor4f(c); });
//   return res;
// }

template <typename T>
void readExr(const std::filesystem::path &input, T *output) {
  DEBUG("read exr file: {}", input);
  Imf::RgbaInputFile file(input.c_str());
  // support for Cropped image reading and writing
  Imath::Box2i dw = file.dataWindow();
  int width = dw.max.x - dw.min.x + 1;
  int height = dw.max.y - dw.min.y + 1;
  ImageData4h pixels(height, width);
  file.setFrameBuffer(pixels.data() - dw.min.x - dw.min.y * width, 1, width);
  file.readPixels(dw.min.y, dw.max.y);
  if (!output) {
    return;
  }
  if constexpr (std::is_same_v<T, ImageData4h>) {
    *output = std::move(pixels);
  } else if constexpr (std::is_same_v<T, ImageData4f>) {
    *output = pixels.unaryExpr([](const Imf::Rgba &c) { return toColor4f(c); });
  } else if constexpr (std::is_same_v<T, ImageData4u8>) {
    *output = pixels.unaryExpr(
        [](const Imf::Rgba &c) { return toUint8(toSrgb(toColor4f(c))); });
  }
}

void savePng(const std::filesystem::path &output, const ImageData4u8 &pic) {
  DEBUG("save png file: {}", output);
  int success =
      stbi_write_png(output.c_str(), pic.cols(), pic.rows(), 4, pic.data(), 0);
  if (!success) {
    throw RuntimeError("failed to save png: {}", output);
  }
}

void saveExr(const std::filesystem::path &output, const ImageData4f &pic) {
  DEBUG("save exr file: {}", output);
  ImageData4h pic_4h =
      pic.unaryExpr([](const Color4f &c) { return toImfRgba(c); });
  Imf::RgbaOutputFile file(output.c_str(), pic.cols(), pic.rows(),
                           Imf::WRITE_RGBA);
  file.setFrameBuffer(pic_4h.data(), 1, pic_4h.cols());
  file.writePixels(pic_4h.rows());
}

void saveExr(const std::filesystem::path &output, int display_height,
             int display_width, int row_offset, int col_offset,
             const ImageData4f &block) {
  DEBUG("save exr file (sub): {}", output);
  lumos::ImageData4h block_data =
      block.unaryExpr([](const Color4f &c) { return lumos::toImfRgba(c); });
  int dw_width = block.cols();
  int dw_height = block.rows();
  Imath::Box2i display_window{{0, 0}, {display_width - 1, display_height - 1}};
  Imath::Box2i data_window{
      {col_offset, row_offset},
      {col_offset + dw_width - 1, row_offset + dw_height - 1}};
  Imf::RgbaOutputFile file(output.c_str(), display_window, data_window,
                           Imf::WRITE_RGBA);
  file.setFrameBuffer(block_data.data() - data_window.min.x -
                          data_window.min.y * dw_width,
                      1, dw_width);
  file.writePixels(dw_height);
}

template void readExr<ImageData4h>(const std::filesystem::path &,
                                   ImageData4h *);
template void readExr<ImageData4f>(const std::filesystem::path &,
                                   ImageData4f *);
template void readExr<ImageData4u8>(const std::filesystem::path &,
                                    ImageData4u8 *);
} // namespace lumos