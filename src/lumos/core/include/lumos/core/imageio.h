#pragma once

#include "lumos/core/common.h"
#include <ImathBox.h>
#include <filesystem>

namespace lumos {
void readPng(const std::filesystem::path &input, ImageData4u8* output);

template<typename T>
void readExr(const std::filesystem::path &input, T* output);

// TODO: add support for ImageDataView series and Block ?
void savePng(const std::filesystem::path &output,
                    const ImageData4u8 &pic);

void saveExr(const std::filesystem::path &output,
                    const ImageData4f &pic);

void saveExr(const std::filesystem::path &output, int display_height,
                    int display_width, int row_offset, int col_offset,
                    const ImageData4f &block);
} // namespace lumos