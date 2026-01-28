#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

namespace lumina::ui
{
    using ui_id = unsigned int;

    // --- ID ---

    inline ui_id get_id(const char* str_id) { return ImGui::GetID(str_id); }
    inline ui_id get_id(const void* ptr_id) { return ImGui::GetID(ptr_id); }
    inline ui_id get_id(int int_id) { return ImGui::GetID(int_id); }

    // --- Item state ---

    inline bool is_item_clicked(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsItemClicked(button); }
    inline bool is_item_double_clicked(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsItemClicked(button) && ImGui::IsMouseDoubleClicked(button); }
    inline bool is_item_hovered(ImGuiHoveredFlags flags = 0) { return ImGui::IsItemHovered(flags); }
    inline bool is_item_active() { return ImGui::IsItemActive(); }
    inline bool is_item_focused() { return ImGui::IsItemFocused(); }
    inline bool is_item_edited() { return ImGui::IsItemEdited(); }
    inline bool is_item_activated() { return ImGui::IsItemActivated(); }
    inline bool is_item_deactivated() { return ImGui::IsItemDeactivated(); }
    inline bool is_item_deactivated_after_edit() { return ImGui::IsItemDeactivatedAfterEdit(); }
    inline bool is_item_toggled_open() { return ImGui::IsItemToggledOpen(); }
    inline bool is_item_visible() { return ImGui::IsItemVisible(); }

    // --- Item info ---

    inline ui_id get_item_id() { return ImGui::GetItemID(); }
    inline ImVec2 get_item_rect_min() { return ImGui::GetItemRectMin(); }
    inline ImVec2 get_item_rect_max() { return ImGui::GetItemRectMax(); }
    inline ImVec2 get_item_rect_size() { return ImGui::GetItemRectSize(); }

    // --- Any item state ---

    inline bool is_any_item_hovered() { return ImGui::IsAnyItemHovered(); }
    inline bool is_any_item_active() { return ImGui::IsAnyItemActive(); }
    inline bool is_any_item_focused() { return ImGui::IsAnyItemFocused(); }

    // --- Window state ---

    inline bool is_window_focused(ImGuiFocusedFlags flags = 0) { return ImGui::IsWindowFocused(flags); }
    inline bool is_window_hovered(ImGuiHoveredFlags flags = 0) { return ImGui::IsWindowHovered(flags); }
    inline bool is_window_appearing() { return ImGui::IsWindowAppearing(); }
    inline bool is_window_collapsed() { return ImGui::IsWindowCollapsed(); }

    inline ImVec2 get_window_pos() { return ImGui::GetWindowPos(); }
    inline ImVec2 get_window_size() { return ImGui::GetWindowSize(); }
    inline float get_window_width() { return ImGui::GetWindowWidth(); }
    inline float get_window_height() { return ImGui::GetWindowHeight(); }

    // --- Cursor / layout position ---

    inline ImVec2 get_cursor_pos() { return ImGui::GetCursorPos(); }
    inline float get_cursor_pos_x() { return ImGui::GetCursorPosX(); }
    inline float get_cursor_pos_y() { return ImGui::GetCursorPosY(); }
    inline void set_cursor_pos(const ImVec2& pos) { ImGui::SetCursorPos(pos); }
    inline void set_cursor_pos(const glm::vec2& pos) { ImGui::SetCursorPos(ImVec2(pos.x, pos.y)); }
    inline void set_cursor_pos_x(float x) { ImGui::SetCursorPosX(x); }
    inline void set_cursor_pos_y(float y) { ImGui::SetCursorPosY(y); }

    inline ImVec2 get_cursor_screen_pos() { return ImGui::GetCursorScreenPos(); }
    inline void set_cursor_screen_pos(const ImVec2& pos) { ImGui::SetCursorScreenPos(pos); }
    inline void set_cursor_screen_pos(const glm::vec2& pos) { ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y)); }

    // --- Focus ---

    inline void set_keyboard_focus_here(int offset = 0) { ImGui::SetKeyboardFocusHere(offset); }
    inline void set_item_default_focus() { ImGui::SetItemDefaultFocus(); }

    // --- Scroll ---

    inline float get_scroll_x() { return ImGui::GetScrollX(); }
    inline float get_scroll_y() { return ImGui::GetScrollY(); }
    inline float get_scroll_max_x() { return ImGui::GetScrollMaxX(); }
    inline float get_scroll_max_y() { return ImGui::GetScrollMaxY(); }
    inline void set_scroll_x(float scroll_x) { ImGui::SetScrollX(scroll_x); }
    inline void set_scroll_y(float scroll_y) { ImGui::SetScrollY(scroll_y); }
    inline void set_scroll_here_x(float center_ratio = 0.5f) { ImGui::SetScrollHereX(center_ratio); }
    inline void set_scroll_here_y(float center_ratio = 0.5f) { ImGui::SetScrollHereY(center_ratio); }

    // --- Keyboard input ---

    inline bool is_key_pressed(ImGuiKey key, bool repeat = true) { return ImGui::IsKeyPressed(key, repeat); }
    inline bool is_key_down(ImGuiKey key) { return ImGui::IsKeyDown(key); }
    inline bool is_key_released(ImGuiKey key) { return ImGui::IsKeyReleased(key); }

    // --- Mouse input ---

    inline bool is_mouse_clicked(ImGuiMouseButton button = ImGuiMouseButton_Left, bool repeat = false) { return ImGui::IsMouseClicked(button, repeat); }
    inline bool is_mouse_double_clicked(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsMouseDoubleClicked(button); }
    inline bool is_mouse_down(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsMouseDown(button); }
    inline bool is_mouse_released(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsMouseReleased(button); }
    inline bool is_mouse_dragging(ImGuiMouseButton button = ImGuiMouseButton_Left, float lock_threshold = -1.0f) { return ImGui::IsMouseDragging(button, lock_threshold); }

    inline ImVec2 get_mouse_pos() { return ImGui::GetMousePos(); }
    inline ImVec2 get_mouse_drag_delta(ImGuiMouseButton button = ImGuiMouseButton_Left, float lock_threshold = -1.0f) { return ImGui::GetMouseDragDelta(button, lock_threshold); }
    inline void reset_mouse_drag_delta(ImGuiMouseButton button = ImGuiMouseButton_Left) { ImGui::ResetMouseDragDelta(button); }

    // --- Clipboard ---

    inline const char* get_clipboard_text() { return ImGui::GetClipboardText(); }
    inline void set_clipboard_text(const char* text) { ImGui::SetClipboardText(text); }
}
