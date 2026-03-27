#pragma once

#include <imgui.h>

#include <concepts>
#include <vector>

namespace Lumina::UI
{
    // Table() scope is in scoped.h. This file adds column/row helpers.

    inline void TableSetupColumn(const char* label, ImGuiTableColumnFlags flags = 0, float initWidth = 0.0f)
    {
        ImGui::TableSetupColumn(label, flags, initWidth);
    }

    inline void TableHeadersRow()
    {
        ImGui::TableHeadersRow();
    }

    inline void TableNextRow(ImGuiTableRowFlags flags = 0, float minHeight = 0.0f)
    {
        ImGui::TableNextRow(flags, minHeight);
    }

    inline bool TableNextColumn()
    {
        return ImGui::TableNextColumn();
    }

    inline bool TableSetColumn(int index)
    {
        return ImGui::TableSetColumnIndex(index);
    }

    template<typename T, std::invocable<const T&, int> F>
    void TableRows(const std::vector<T>& items, F&& rowFn)
    {
        for (int i = 0; i < static_cast<int>(items.size()); ++i)
        {
            ImGui::TableNextRow();
            rowFn(items[i], i);
        }
    }
}
