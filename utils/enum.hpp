#pragma

#include <imgui.h>
#include <.lib/enum/enum_combo.hpp>

template<typename E>
bool combo(const char* label, E& value) {
    static const auto names = reflect::names<E>();
    const int current = static_cast<int>(value);

    bool changed = false;
    if(ImGui::BeginCombo(label, names[std::size_t(current)].data())) {
        enum_combo<E>([&](std::string_view name, E candidate) {
            const bool selected = (static_cast<int>(candidate) == current);
            if(ImGui::Selectable(name.data(), selected)) {
                value = candidate;
                changed = true;
            }
            if(selected) ImGui::SetItemDefaultFocus;
        });
        ImGui::EndCombo();
    }

    return changed;
}