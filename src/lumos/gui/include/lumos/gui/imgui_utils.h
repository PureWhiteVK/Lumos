#pragma once

#include <imgui.h>

namespace lumos {
namespace gui {

void HelpMarker(const char *desc);
void ImageTooltip(ImTextureID texture_id, const ImVec2 &relateive_pos,
                  const ImVec2 &image_size,
                  const ImVec4 &tint_color = {1, 1, 1, 1},
                  const ImVec4 &boarder_color = {0, 0, 0, 0});
}
} // namespace lumos