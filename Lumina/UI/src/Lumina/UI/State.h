#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

namespace Lumina::UI
{
    using UIId = unsigned int;

    // --- ID ---

    [[nodiscard]] inline UIId GetID(const char* strId) { return ImGui::GetID(strId); }
    [[nodiscard]] inline UIId GetID(const void* ptrId) { return ImGui::GetID(ptrId); }
    [[nodiscard]] inline UIId GetID(int intId) { return ImGui::GetID(intId); }

    // --- Item state ---

    [[nodiscard]] inline bool IsItemClicked(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsItemClicked(button); }
    [[nodiscard]] inline bool IsItemDoubleClicked(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsItemClicked(button) && ImGui::IsMouseDoubleClicked(button); }
    [[nodiscard]] inline bool IsItemHovered(ImGuiHoveredFlags flags = 0) { return ImGui::IsItemHovered(flags); }
    [[nodiscard]] inline bool IsItemActive() { return ImGui::IsItemActive(); }
    [[nodiscard]] inline bool IsItemFocused() { return ImGui::IsItemFocused(); }
    [[nodiscard]] inline bool IsItemEdited() { return ImGui::IsItemEdited(); }
    [[nodiscard]] inline bool IsItemActivated() { return ImGui::IsItemActivated(); }
    [[nodiscard]] inline bool IsItemDeactivated() { return ImGui::IsItemDeactivated(); }
    [[nodiscard]] inline bool IsItemDeactivatedAfterEdit() { return ImGui::IsItemDeactivatedAfterEdit(); }
    [[nodiscard]] inline bool IsItemToggledOpen() { return ImGui::IsItemToggledOpen(); }
    [[nodiscard]] inline bool IsItemVisible() { return ImGui::IsItemVisible(); }

    // --- Item info ---

    [[nodiscard]] inline UIId GetItemID() { return ImGui::GetItemID(); }
    [[nodiscard]] inline ImVec2 GetItemRectMin() { return ImGui::GetItemRectMin(); }
    [[nodiscard]] inline ImVec2 GetItemRectMax() { return ImGui::GetItemRectMax(); }
    [[nodiscard]] inline ImVec2 GetItemRectSize() { return ImGui::GetItemRectSize(); }

    // --- Any item state ---

    [[nodiscard]] inline bool IsAnyItemHovered() { return ImGui::IsAnyItemHovered(); }
    [[nodiscard]] inline bool IsAnyItemActive() { return ImGui::IsAnyItemActive(); }
    [[nodiscard]] inline bool IsAnyItemFocused() { return ImGui::IsAnyItemFocused(); }

    // --- Window state ---

    [[nodiscard]] inline bool IsWindowFocused(ImGuiFocusedFlags flags = 0) { return ImGui::IsWindowFocused(flags); }
    [[nodiscard]] inline bool IsWindowHovered(ImGuiHoveredFlags flags = 0) { return ImGui::IsWindowHovered(flags); }
    [[nodiscard]] inline bool IsWindowAppearing() { return ImGui::IsWindowAppearing(); }
    [[nodiscard]] inline bool IsWindowCollapsed() { return ImGui::IsWindowCollapsed(); }

    [[nodiscard]] inline ImVec2 GetWindowPos() { return ImGui::GetWindowPos(); }
    [[nodiscard]] inline ImVec2 GetWindowSize() { return ImGui::GetWindowSize(); }
    [[nodiscard]] inline float GetWindowWidth() { return ImGui::GetWindowWidth(); }
    [[nodiscard]] inline float GetWindowHeight() { return ImGui::GetWindowHeight(); }

    // --- Cursor / layout position ---

    [[nodiscard]] inline ImVec2 GetCursorPos() { return ImGui::GetCursorPos(); }
    [[nodiscard]] inline float GetCursorPosX() { return ImGui::GetCursorPosX(); }
    [[nodiscard]] inline float GetCursorPosY() { return ImGui::GetCursorPosY(); }
    inline void SetCursorPos(const ImVec2& pos) { ImGui::SetCursorPos(pos); }
    inline void SetCursorPos(const glm::vec2& pos) { ImGui::SetCursorPos(ImVec2(pos.x, pos.y)); }
    inline void SetCursorPosX(float x) { ImGui::SetCursorPosX(x); }
    inline void SetCursorPosY(float y) { ImGui::SetCursorPosY(y); }

    [[nodiscard]] inline ImVec2 GetCursorScreenPos() { return ImGui::GetCursorScreenPos(); }
    inline void SetCursorScreenPos(const ImVec2& pos) { ImGui::SetCursorScreenPos(pos); }
    inline void SetCursorScreenPos(const glm::vec2& pos) { ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y)); }

    // --- Focus ---

    inline void SetKeyboardFocusHere(int offset = 0) { ImGui::SetKeyboardFocusHere(offset); }
    inline void SetItemDefaultFocus() { ImGui::SetItemDefaultFocus(); }

    // --- Scroll ---

    [[nodiscard]] inline float GetScrollX() { return ImGui::GetScrollX(); }
    [[nodiscard]] inline float GetScrollY() { return ImGui::GetScrollY(); }
    [[nodiscard]] inline float GetScrollMaxX() { return ImGui::GetScrollMaxX(); }
    [[nodiscard]] inline float GetScrollMaxY() { return ImGui::GetScrollMaxY(); }
    inline void SetScrollX(float scrollX) { ImGui::SetScrollX(scrollX); }
    inline void SetScrollY(float scrollY) { ImGui::SetScrollY(scrollY); }
    inline void SetScrollHereX(float centerRatio = 0.5f) { ImGui::SetScrollHereX(centerRatio); }
    inline void SetScrollHereY(float centerRatio = 0.5f) { ImGui::SetScrollHereY(centerRatio); }

    // --- Keyboard input ---

    [[nodiscard]] inline bool IsKeyPressed(ImGuiKey key, bool repeat = true) { return ImGui::IsKeyPressed(key, repeat); }
    [[nodiscard]] inline bool IsKeyDown(ImGuiKey key) { return ImGui::IsKeyDown(key); }
    [[nodiscard]] inline bool IsKeyReleased(ImGuiKey key) { return ImGui::IsKeyReleased(key); }

    // --- Mouse input ---

    [[nodiscard]] inline bool IsMouseClicked(ImGuiMouseButton button = ImGuiMouseButton_Left, bool repeat = false) { return ImGui::IsMouseClicked(button, repeat); }
    [[nodiscard]] inline bool IsMouseDoubleClicked(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsMouseDoubleClicked(button); }
    [[nodiscard]] inline bool IsMouseDown(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsMouseDown(button); }
    [[nodiscard]] inline bool IsMouseReleased(ImGuiMouseButton button = ImGuiMouseButton_Left) { return ImGui::IsMouseReleased(button); }
    [[nodiscard]] inline bool IsMouseDragging(ImGuiMouseButton button = ImGuiMouseButton_Left, float lockThreshold = -1.0f) { return ImGui::IsMouseDragging(button, lockThreshold); }

    [[nodiscard]] inline ImVec2 GetMousePos() { return ImGui::GetMousePos(); }
    [[nodiscard]] inline ImVec2 GetMouseDragDelta(ImGuiMouseButton button = ImGuiMouseButton_Left, float lockThreshold = -1.0f) { return ImGui::GetMouseDragDelta(button, lockThreshold); }
    inline void ResetMouseDragDelta(ImGuiMouseButton button = ImGuiMouseButton_Left) { ImGui::ResetMouseDragDelta(button); }

    // --- Clipboard ---

    [[nodiscard]] inline const char* GetClipboardText() { return ImGui::GetClipboardText(); }
    inline void SetClipboardText(const char* text) { ImGui::SetClipboardText(text); }
}
