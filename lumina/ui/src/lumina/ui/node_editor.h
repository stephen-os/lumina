#pragma once

#include <imgui_node_editor.h>

#include <memory>

namespace lumina::ui
{
    // Bring node editor types into ui namespace for convenience
    namespace ne = ax::NodeEditor;

    using node_id = ne::NodeId;
    using link_id = ne::LinkId;
    using pin_id = ne::PinId;
    using pin_kind = ne::PinKind;
    using ne_config = ne::Config;
    using ne_style = ne::Style;

    // Editor context wrapper with RAII
    class node_editor_context
    {
    public:
        node_editor_context(const ne_config* config = nullptr)
            : m_context(ne::CreateEditor(config))
        {
        }

        ~node_editor_context()
        {
            if (m_context)
            {
                ne::DestroyEditor(m_context);
            }
        }

        node_editor_context(const node_editor_context&) = delete;
        node_editor_context& operator=(const node_editor_context&) = delete;

        node_editor_context(node_editor_context&& other) noexcept
            : m_context(other.m_context)
        {
            other.m_context = nullptr;
        }

        node_editor_context& operator=(node_editor_context&& other) noexcept
        {
            if (this != &other)
            {
                if (m_context) ne::DestroyEditor(m_context);
                m_context = other.m_context;
                other.m_context = nullptr;
            }
            return *this;
        }

        void set_current() { ne::SetCurrentEditor(m_context); }
        ne::EditorContext* get() { return m_context; }
        operator bool() const { return m_context != nullptr; }

    private:
        ne::EditorContext* m_context = nullptr;
    };

    // Node editor immediate mode API wrappers
    inline void ne_set_current(ne::EditorContext* ctx) { ne::SetCurrentEditor(ctx); }
    inline ne::EditorContext* ne_get_current() { return ne::GetCurrentEditor(); }
    inline ne_style& ne_get_style() { return ne::GetStyle(); }

    inline void ne_begin(const char* id, const ImVec2& size = ImVec2(0, 0)) { ne::Begin(id, size); }
    inline void ne_end() { ne::End(); }

    inline void ne_begin_node(node_id id) { ne::BeginNode(id); }
    inline void ne_end_node() { ne::EndNode(); }

    inline void ne_begin_pin(pin_id id, pin_kind kind) { ne::BeginPin(id, kind); }
    inline void ne_end_pin() { ne::EndPin(); }

    inline bool ne_link(link_id id, pin_id start, pin_id end,
                        const ImVec4& color = ImVec4(1, 1, 1, 1), float thickness = 1.0f)
    {
        return ne::Link(id, start, end, color, thickness);
    }

    // Creation
    inline bool ne_begin_create(const ImVec4& color = ImVec4(1, 1, 1, 1), float thickness = 1.0f)
    {
        return ne::BeginCreate(color, thickness);
    }
    inline bool ne_query_new_link(pin_id* start, pin_id* end) { return ne::QueryNewLink(start, end); }
    inline bool ne_query_new_node(pin_id* pin) { return ne::QueryNewNode(pin); }
    inline bool ne_accept_new_item() { return ne::AcceptNewItem(); }
    inline void ne_reject_new_item() { ne::RejectNewItem(); }
    inline void ne_end_create() { ne::EndCreate(); }

    // Deletion
    inline bool ne_begin_delete() { return ne::BeginDelete(); }
    inline bool ne_query_deleted_link(link_id* id, pin_id* start = nullptr, pin_id* end = nullptr)
    {
        return ne::QueryDeletedLink(id, start, end);
    }
    inline bool ne_query_deleted_node(node_id* id) { return ne::QueryDeletedNode(id); }
    inline bool ne_accept_deleted_item(bool delete_deps = true) { return ne::AcceptDeletedItem(delete_deps); }
    inline void ne_reject_deleted_item() { ne::RejectDeletedItem(); }
    inline void ne_end_delete() { ne::EndDelete(); }

    // Node manipulation
    inline void ne_set_node_position(node_id id, const ImVec2& pos) { ne::SetNodePosition(id, pos); }
    inline ImVec2 ne_get_node_position(node_id id) { return ne::GetNodePosition(id); }
    inline ImVec2 ne_get_node_size(node_id id) { return ne::GetNodeSize(id); }
    inline void ne_center_node_on_screen(node_id id) { ne::CenterNodeOnScreen(id); }

    // Selection
    inline int ne_get_selected_object_count() { return ne::GetSelectedObjectCount(); }
    inline int ne_get_selected_nodes(node_id* nodes, int size) { return ne::GetSelectedNodes(nodes, size); }
    inline int ne_get_selected_links(link_id* links, int size) { return ne::GetSelectedLinks(links, size); }
    inline void ne_clear_selection() { ne::ClearSelection(); }
    inline void ne_select_node(node_id id, bool append = false) { ne::SelectNode(id, append); }
    inline void ne_select_link(link_id id, bool append = false) { ne::SelectLink(id, append); }
    inline bool ne_delete_node(node_id id) { return ne::DeleteNode(id); }
    inline bool ne_delete_link(link_id id) { return ne::DeleteLink(id); }

    // Context menus
    inline bool ne_show_node_context_menu(node_id* id) { return ne::ShowNodeContextMenu(id); }
    inline bool ne_show_pin_context_menu(pin_id* id) { return ne::ShowPinContextMenu(id); }
    inline bool ne_show_link_context_menu(link_id* id) { return ne::ShowLinkContextMenu(id); }
    inline bool ne_show_background_context_menu() { return ne::ShowBackgroundContextMenu(); }

    // Navigation
    inline void ne_navigate_to_content(float duration = -1) { ne::NavigateToContent(duration); }
    inline void ne_navigate_to_selection(bool zoom_in = false, float duration = -1)
    {
        ne::NavigateToSelection(zoom_in, duration);
    }

    // Suspend/Resume for overlays
    inline void ne_suspend() { ne::Suspend(); }
    inline void ne_resume() { ne::Resume(); }

    // Queries
    inline node_id ne_get_hovered_node() { return ne::GetHoveredNode(); }
    inline pin_id ne_get_hovered_pin() { return ne::GetHoveredPin(); }
    inline link_id ne_get_hovered_link() { return ne::GetHoveredLink(); }
    inline node_id ne_get_double_clicked_node() { return ne::GetDoubleClickedNode(); }
    inline bool ne_is_background_clicked() { return ne::IsBackgroundClicked(); }
    inline float ne_get_current_zoom() { return ne::GetCurrentZoom(); }

    // Coordinate conversion
    inline ImVec2 ne_screen_to_canvas(const ImVec2& pos) { return ne::ScreenToCanvas(pos); }
    inline ImVec2 ne_canvas_to_screen(const ImVec2& pos) { return ne::CanvasToScreen(pos); }

    // Flow animation
    inline void ne_flow(link_id id, ne::FlowDirection dir = ne::FlowDirection::Forward) { ne::Flow(id, dir); }
}
