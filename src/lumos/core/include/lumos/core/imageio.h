#include "lumos/core/common.h"
#include <ImathBox.h>
#include <filesystem>

namespace lumos {
extern ImageData4u8 readPng(const std::filesystem::path &input);

extern ImageData4f readExr(const std::filesystem::path &input);

// TODO: add support for ImageDataView series and Block ?
extern void savePng(const std::filesystem::path &output,
                    const ImageData4u8 &pic);

extern void saveExr(const std::filesystem::path &output,
                    const ImageData4f &pic);

extern void saveExr(const std::filesystem::path &output, int display_height,
                    int display_width, int row_offset, int col_offset,
                    const ImageData4f &block);
} // namespace lumos