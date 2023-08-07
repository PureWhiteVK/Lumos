#pragma once

#include <imgui.h>

namespace lumos {
namespace gui {

void helpMarker(const char *desc);
void imageTooltip(ImTextureID texture_id, const ImVec2 &relateive_pos,
                  const ImVec2 &image_size,
                  const ImVec4 &tint_color = {1, 1, 1, 1},
                  const ImVec4 &boarder_color = {0, 0, 0, 0});
}
} // namespace lumos