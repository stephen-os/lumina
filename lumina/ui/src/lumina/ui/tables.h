#pragma once

#include <imgui.h>

#include <concepts>
#include <vector>

namespace lumina::ui
{
    // table() scope is in scoped.h. This file adds column/row helpers.

    inline void table_setup_column(const char* label, ImGuiTableColumnFlags flags = 0, float init_width = 0.0f)
    {
        ImGui::TableSetupColumn(label, flags, init_width);
    }

    inline void table_headers_row()
    {
        ImGui::TableHeadersRow();
    }

    inline void table_next_row(ImGuiTableRowFlags flags = 0, float min_height = 0.0f)
    {
        ImGui::TableNextRow(flags, min_height);
    }

    inline bool table_next_column()
    {
        return ImGui::TableNextColumn();
    }

    inline bool table_set_column(int index)
    {
        return ImGui::TableSetColumnIndex(index);
    }

    template<typename T, std::invocable<const T&, int> F>
    void table_rows(const std::vector<T>& items, F&& row_fn)
    {
        for (int i = 0; i < static_cast<int>(items.size()); ++i)
        {
            ImGui::TableNextRow();
            row_fn(items[i], i);
        }
    }
}
