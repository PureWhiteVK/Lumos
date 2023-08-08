#pragma once

#include "lumos/core/common.h"
#include <ImathBox.h>
#include <filesystem>

namespace lumos {
void ReadPng(const std::filesystem::path &input, ImageData4u8* output);

template<typename T>
void ReadExr(const std::filesystem::path &input, T* output);

// TODO: add support for ImageDataView series and Block ?
void SavePng(const std::filesystem::path &output,
                    const ImageData4u8 &pic);

void SaveExr(const std::filesystem::path &output,
                    const ImageData4f &pic);

void SaveExr(const std::filesystem::path &output, int display_height,
                    int display_width, int row_offset, int col_offset,
                    const ImageData4f &block);
} // namespace lumos