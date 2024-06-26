#include "user_interface.hpp"

auto
UI::draw() -> void
{
    this->top_menu();
    this->toolset();
}

auto
UI::top_menu() -> void
{
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text("ClosedGL");

        if (ImGui::BeginMenu("File")) {
            // TODO: bind these items to functions

            if (ImGui::MenuItem("Open...", nullptr)) {
            }

            if (ImGui::MenuItem("Export...", nullptr)) {
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", nullptr)) {
                glfwSetWindowShouldClose(this->window, 1);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Toolbox", nullptr, &this->show_toolset);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

auto
UI::toolset() -> void
{
    if (this->show_toolset) {
        ImGui::Begin("Toolbox");

        ImGui::End();
    }
}

auto
UI::draw_debug(float delta_time, std::string_view info) -> void
{
    ImGui::Begin("Debug Window");
    ImGui::Text("ClosedGl Debug");
    ImGui::Text("%s", info.data());
    ImGui::Text("frames: %d", static_cast<int>(1.0F / delta_time));
    ImGui::Text("frame time: %f second", delta_time);
    ImGui::End();
}