#include "lumos/gui/imgui_utils.h"

namespace lumos {
namespace gui {
void HelpMarker(const char *desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void ImageTooltip(ImTextureID texture_id, const ImVec2 &relateive_pos,
                  const ImVec2 &image_size, const ImVec4 &tint_color,
                  const ImVec4 &boarder_color) {
  const ImGuiIO &io = ImGui::GetIO();
  if (ImGui::BeginItemTooltip()) {
    static ImVec2 uv0, uv1;
    static float zoom = 4.0f;
    static float region_sz = 32.0f, region_x, region_y;
    region_x = relateive_pos.x - region_sz * 0.5f;
    region_y = relateive_pos.y - region_sz * 0.5f;
    if (region_x < 0.0f) {
      region_x = 0.0f;
    } else if (region_x > image_size.x - region_sz) {
      region_x = image_size.x - region_sz;
    }
    if (region_y < 0.0f) {
      region_y = 0.0f;
    } else if (region_y > image_size.y - region_sz) {
      region_y = image_size.y - region_sz;
    }
    // ImGui::Text("region size: %.2f", region_sz);
    ImGui::Text("Min: (%03.0f, %03.0f)", region_x, region_y);
    ImGui::Text("Max: (%03.0f, %03.0f)", region_x + region_sz,
                region_y + region_sz);
    uv0 = ImVec2((region_x) / image_size.x, (region_y) / image_size.y);
    uv1 = ImVec2((region_x + region_sz) / image_size.x,
                 (region_y + region_sz) / image_size.y);
    ImGui::Image(texture_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0,
                 uv1, tint_color, boarder_color);
    ImGui::EndTooltip();
  }
}
} // namespace gui
} // namespace lumos