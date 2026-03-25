#pragma once

#include <imgui_node_editor.h>
#include <glm/glm.hpp>

#include <memory>

namespace Lumina::UI
{
    // Bring node editor types into ui namespace for convenience
    namespace NE = ax::NodeEditor;

    using NodeId = NE::NodeId;
    using LinkId = NE::LinkId;
    using PinId = NE::PinId;
    using PinKind = NE::PinKind;
    using NEConfig = NE::Config;
    using NEStyle = NE::Style;

    // Editor context wrapper with RAII
    class NodeEditorContext
    {
    public:
        NodeEditorContext(const NEConfig* config = nullptr)
            : m_Context(NE::CreateEditor(config))
        {
        }

        ~NodeEditorContext()
        {
            if (m_Context)
            {
                NE::DestroyEditor(m_Context);
            }
        }

        NodeEditorContext(const NodeEditorContext&) = delete;
        NodeEditorContext& operator=(const NodeEditorContext&) = delete;

        NodeEditorContext(NodeEditorContext&& other) noexcept
            : m_Context(other.m_Context)
        {
            other.m_Context = nullptr;
        }

        NodeEditorContext& operator=(NodeEditorContext&& other) noexcept
        {
            if (this != &other)
            {
                if (m_Context) NE::DestroyEditor(m_Context);
                m_Context = other.m_Context;
                other.m_Context = nullptr;
            }
            return *this;
        }

        void SetCurrent() { NE::SetCurrentEditor(m_Context); }
        [[nodiscard]] NE::EditorContext* Get() { return m_Context; }
        [[nodiscard]] operator bool() const { return m_Context != nullptr; }

    private:
        NE::EditorContext* m_Context = nullptr;
    };

    // Node editor immediate mode API wrappers
    inline void NESetCurrent(NE::EditorContext* ctx) { NE::SetCurrentEditor(ctx); }
    [[nodiscard]] inline NE::EditorContext* NEGetCurrent() { return NE::GetCurrentEditor(); }
    [[nodiscard]] inline NEStyle& NEGetStyle() { return NE::GetStyle(); }

    inline void NEBegin(const char* id, const ImVec2& size = ImVec2(0, 0)) { NE::Begin(id, size); }
    inline void NEBegin(const char* id, const glm::vec2& size) { NE::Begin(id, ImVec2(size.x, size.y)); }
    inline void NEEnd() { NE::End(); }

    inline void NEBeginNode(NodeId id) { NE::BeginNode(id); }
    inline void NEEndNode() { NE::EndNode(); }

    inline void NEBeginPin(PinId id, PinKind kind) { NE::BeginPin(id, kind); }
    inline void NEEndPin() { NE::EndPin(); }

    [[nodiscard]] inline bool NELink(LinkId id, PinId start, PinId end,
                        const ImVec4& color = ImVec4(1, 1, 1, 1), float thickness = 1.0f)
    {
        return NE::Link(id, start, end, color, thickness);
    }
    [[nodiscard]] inline bool NELink(LinkId id, PinId start, PinId end,
                        const glm::vec4& color, float thickness = 1.0f)
    {
        return NE::Link(id, start, end, ImVec4(color.x, color.y, color.z, color.w), thickness);
    }

    // Creation
    [[nodiscard]] inline bool NEBeginCreate(const ImVec4& color = ImVec4(1, 1, 1, 1), float thickness = 1.0f)
    {
        return NE::BeginCreate(color, thickness);
    }
    [[nodiscard]] inline bool NEBeginCreate(const glm::vec4& color, float thickness = 1.0f)
    {
        return NE::BeginCreate(ImVec4(color.x, color.y, color.z, color.w), thickness);
    }
    [[nodiscard]] inline bool NEQueryNewLink(PinId* start, PinId* end) { return NE::QueryNewLink(start, end); }
    [[nodiscard]] inline bool NEQueryNewNode(PinId* pin) { return NE::QueryNewNode(pin); }
    [[nodiscard]] inline bool NEAcceptNewItem() { return NE::AcceptNewItem(); }
    inline void NERejectNewItem() { NE::RejectNewItem(); }
    inline void NEEndCreate() { NE::EndCreate(); }

    // Deletion
    [[nodiscard]] inline bool NEBeginDelete() { return NE::BeginDelete(); }
    [[nodiscard]] inline bool NEQueryDeletedLink(LinkId* id, PinId* start = nullptr, PinId* end = nullptr)
    {
        return NE::QueryDeletedLink(id, start, end);
    }
    [[nodiscard]] inline bool NEQueryDeletedNode(NodeId* id) { return NE::QueryDeletedNode(id); }
    [[nodiscard]] inline bool NEAcceptDeletedItem(bool deleteDeps = true) { return NE::AcceptDeletedItem(deleteDeps); }
    inline void NERejectDeletedItem() { NE::RejectDeletedItem(); }
    inline void NEEndDelete() { NE::EndDelete(); }

    // Node manipulation
    inline void NESetNodePosition(NodeId id, const ImVec2& pos) { NE::SetNodePosition(id, pos); }
    inline void NESetNodePosition(NodeId id, const glm::vec2& pos) { NE::SetNodePosition(id, ImVec2(pos.x, pos.y)); }
    [[nodiscard]] inline ImVec2 NEGetNodePosition(NodeId id) { return NE::GetNodePosition(id); }
    [[nodiscard]] inline glm::vec2 NEGetNodePositionVec2(NodeId id) { auto p = NE::GetNodePosition(id); return {p.x, p.y}; }
    [[nodiscard]] inline ImVec2 NEGetNodeSize(NodeId id) { return NE::GetNodeSize(id); }
    [[nodiscard]] inline glm::vec2 NEGetNodeSizeVec2(NodeId id) { auto s = NE::GetNodeSize(id); return {s.x, s.y}; }
    inline void NECenterNodeOnScreen(NodeId id) { NE::CenterNodeOnScreen(id); }

    // Selection
    [[nodiscard]] inline int NEGetSelectedObjectCount() { return NE::GetSelectedObjectCount(); }
    [[nodiscard]] inline int NEGetSelectedNodes(NodeId* nodes, int size) { return NE::GetSelectedNodes(nodes, size); }
    [[nodiscard]] inline int NEGetSelectedLinks(LinkId* links, int size) { return NE::GetSelectedLinks(links, size); }
    inline void NEClearSelection() { NE::ClearSelection(); }
    inline void NESelectNode(NodeId id, bool append = false) { NE::SelectNode(id, append); }
    inline void NESelectLink(LinkId id, bool append = false) { NE::SelectLink(id, append); }
    [[nodiscard]] inline bool NEDeleteNode(NodeId id) { return NE::DeleteNode(id); }
    [[nodiscard]] inline bool NEDeleteLink(LinkId id) { return NE::DeleteLink(id); }

    // Context menus
    [[nodiscard]] inline bool NEShowNodeContextMenu(NodeId* id) { return NE::ShowNodeContextMenu(id); }
    [[nodiscard]] inline bool NEShowPinContextMenu(PinId* id) { return NE::ShowPinContextMenu(id); }
    [[nodiscard]] inline bool NEShowLinkContextMenu(LinkId* id) { return NE::ShowLinkContextMenu(id); }
    [[nodiscard]] inline bool NEShowBackgroundContextMenu() { return NE::ShowBackgroundContextMenu(); }

    // Navigation
    inline void NENavigateToContent(float duration = -1) { NE::NavigateToContent(duration); }
    inline void NENavigateToSelection(bool zoomIn = false, float duration = -1)
    {
        NE::NavigateToSelection(zoomIn, duration);
    }

    // Suspend/Resume for overlays
    inline void NESuspend() { NE::Suspend(); }
    inline void NEResume() { NE::Resume(); }

    // Queries
    [[nodiscard]] inline NodeId NEGetHoveredNode() { return NE::GetHoveredNode(); }
    [[nodiscard]] inline PinId NEGetHoveredPin() { return NE::GetHoveredPin(); }
    [[nodiscard]] inline LinkId NEGetHoveredLink() { return NE::GetHoveredLink(); }
    [[nodiscard]] inline NodeId NEGetDoubleClickedNode() { return NE::GetDoubleClickedNode(); }
    [[nodiscard]] inline bool NEIsBackgroundClicked() { return NE::IsBackgroundClicked(); }
    [[nodiscard]] inline float NEGetCurrentZoom() { return NE::GetCurrentZoom(); }

    // Coordinate conversion
    [[nodiscard]] inline ImVec2 NEScreenToCanvas(const ImVec2& pos) { return NE::ScreenToCanvas(pos); }
    [[nodiscard]] inline glm::vec2 NEScreenToCanvas(const glm::vec2& pos) { auto r = NE::ScreenToCanvas(ImVec2(pos.x, pos.y)); return {r.x, r.y}; }
    [[nodiscard]] inline ImVec2 NECanvasToScreen(const ImVec2& pos) { return NE::CanvasToScreen(pos); }
    [[nodiscard]] inline glm::vec2 NECanvasToScreen(const glm::vec2& pos) { auto r = NE::CanvasToScreen(ImVec2(pos.x, pos.y)); return {r.x, r.y}; }

    // Flow animation
    inline void NEFlow(LinkId id, NE::FlowDirection dir = NE::FlowDirection::Forward) { NE::Flow(id, dir); }
}
