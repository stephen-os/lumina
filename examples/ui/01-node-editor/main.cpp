// ui/01-node-editor: Node-based visual editor
// Demonstrates: Node editor, nodes, pins, links, context menus

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <algorithm>

namespace UI = Lumina::UI;

// Simple node structure
struct Node
{
    UI::NodeId Id;
    std::string Name;
    std::vector<UI::PinId> Inputs;
    std::vector<UI::PinId> Outputs;
    glm::vec2 InitialPosition{0, 0};
    bool PositionSet = false;
};

// Simple link structure
struct Link
{
    UI::LinkId Id;
    UI::PinId StartPin;
    UI::PinId EndPin;
};

class NodeEditorLayer : public Lumina::Layer
{
public:
    NodeEditorLayer() : Layer("NodeEditorLayer") {}

    void OnAttach() override
    {
        // Create node editor context
        UI::NEConfig config;
        config.SettingsFile = "ui-01-node-editor.json";
        m_Context = std::make_unique<UI::NodeEditorContext>(&config);

        // Create some example nodes
        CreateNode("Start", 0, 1, {50, 100});
        CreateNode("Process A", 1, 1, {250, 50});
        CreateNode("Process B", 1, 1, {250, 200});
        CreateNode("Combine", 2, 1, {450, 125});
        CreateNode("Output", 1, 0, {650, 125});

        // Create some example links
        CreateLink(m_Nodes[0].Outputs[0], m_Nodes[1].Inputs[0]);
        CreateLink(m_Nodes[0].Outputs[0], m_Nodes[2].Inputs[0]);
        CreateLink(m_Nodes[1].Outputs[0], m_Nodes[3].Inputs[0]);
        CreateLink(m_Nodes[2].Outputs[0], m_Nodes[3].Inputs[1]);
        CreateLink(m_Nodes[3].Outputs[0], m_Nodes[4].Inputs[0]);
    }

    void OnDetach() override
    {
        m_Context.reset();
    }

    void OnRender() override
    {
        UI::BeginWindow("Node Editor");
        UI::Text("Right-click for context menu, drag to connect pins");
        UI::Separator();

        m_Context->SetCurrent();
        UI::NEBegin("Node Editor", glm::vec2(0, 0));

        for (auto& node : m_Nodes)
            DrawNode(node);

        for (auto& link : m_Links)
            UI::NELink(link.Id, link.StartPin, link.EndPin, glm::vec4(1, 1, 1, 1), 2.0f);

        // Handle link creation
        if (UI::NEBeginCreate())
        {
            UI::PinId startPin, endPin;
            if (UI::NEQueryNewLink(&startPin, &endPin))
            {
                if (startPin && endPin && CanCreateLink(startPin, endPin))
                {
                    if (UI::NEAcceptNewItem())
                        CreateLink(startPin, endPin);
                }
                else
                {
                    UI::NERejectNewItem();
                }
            }
        }
        UI::NEEndCreate();

        // Handle deletion
        if (UI::NEBeginDelete())
        {
            UI::LinkId linkId;
            while (UI::NEQueryDeletedLink(&linkId))
                if (UI::NEAcceptDeletedItem())
                    DeleteLink(linkId);

            UI::NodeId nodeId;
            while (UI::NEQueryDeletedNode(&nodeId))
                if (UI::NEAcceptDeletedItem())
                    DeleteNode(nodeId);
        }
        UI::NEEndDelete();

        // Context menus
        UI::NESuspend();
        HandleContextMenus();
        UI::NEResume();

        UI::NEEnd();

        UI::Separator();
        UI::TextFmt("Nodes: {}  Links: {}", m_Nodes.size(), m_Links.size());
        UI::TextFmt("Zoom: {:.2f}x", UI::NEGetCurrentZoom());

        UI::EndWindow();
    }

private:
    void DrawNode(Node& node)
    {
        if (!node.PositionSet)
        {
            UI::NESetNodePosition(node.Id, node.InitialPosition);
            node.PositionSet = true;
        }

        UI::NEBeginNode(node.Id);
        ImGui::Text("%s", node.Name.c_str());

        for (size_t i = 0; i < node.Inputs.size(); ++i)
        {
            UI::NEBeginPin(node.Inputs[i], UI::PinKind::Input);
            ImGui::Text("-> In %zu", i);
            UI::NEEndPin();
        }

        for (size_t i = 0; i < node.Outputs.size(); ++i)
        {
            UI::NEBeginPin(node.Outputs[i], UI::PinKind::Output);
            ImGui::Text("Out %zu ->", i);
            UI::NEEndPin();
        }

        UI::NEEndNode();
    }

    void HandleContextMenus()
    {
        UI::NodeId contextNodeId;
        UI::LinkId contextLinkId;

        if (UI::NEShowNodeContextMenu(&contextNodeId))
        {
            ImGui::OpenPopup("Node Context Menu");
            m_ContextNode = contextNodeId;
        }
        else if (UI::NEShowLinkContextMenu(&contextLinkId))
        {
            ImGui::OpenPopup("Link Context Menu");
            m_ContextLink = contextLinkId;
        }
        else if (UI::NEShowBackgroundContextMenu())
        {
            auto mouse = ImGui::GetMousePos();
            m_ContextMenuPos = {mouse.x, mouse.y};
            ImGui::OpenPopup("Background Context Menu");
        }

        if (ImGui::BeginPopup("Node Context Menu"))
        {
            if (ImGui::MenuItem("Delete Node"))
                DeleteNode(m_ContextNode);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Link Context Menu"))
        {
            if (ImGui::MenuItem("Delete Link"))
                DeleteLink(m_ContextLink);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Background Context Menu"))
        {
            glm::vec2 canvasPos = UI::NEScreenToCanvas(m_ContextMenuPos);
            if (ImGui::MenuItem("Add Process Node"))
                CreateNode("Process", 1, 1, canvasPos);
            if (ImGui::MenuItem("Add Input Node"))
                CreateNode("Input", 0, 1, canvasPos);
            if (ImGui::MenuItem("Add Output Node"))
                CreateNode("Output", 1, 0, canvasPos);
            ImGui::Separator();
            if (ImGui::MenuItem("Navigate to Content"))
                UI::NENavigateToContent();
            ImGui::EndPopup();
        }
    }

    void CreateNode(const std::string& name, int inputCount, int outputCount, glm::vec2 position)
    {
        Node node;
        node.Id = UI::NodeId(m_NextId++);
        node.Name = name;
        for (int i = 0; i < inputCount; ++i)
            node.Inputs.push_back(UI::PinId(m_NextId++));
        for (int i = 0; i < outputCount; ++i)
            node.Outputs.push_back(UI::PinId(m_NextId++));
        node.InitialPosition = position;
        m_Nodes.push_back(node);
    }

    void CreateLink(UI::PinId start, UI::PinId end)
    {
        Link link;
        link.Id = UI::LinkId(m_NextId++);
        link.StartPin = start;
        link.EndPin = end;
        m_Links.push_back(link);
    }

    bool CanCreateLink(UI::PinId start, UI::PinId end)
    {
        Node* startNode = FindNodeByPin(start);
        Node* endNode = FindNodeByPin(end);
        if (!startNode || !endNode || startNode == endNode)
            return false;
        return IsOutputPin(start) != IsOutputPin(end);
    }

    Node* FindNodeByPin(UI::PinId pin)
    {
        for (auto& node : m_Nodes)
        {
            for (auto& p : node.Inputs)
                if (p == pin) return &node;
            for (auto& p : node.Outputs)
                if (p == pin) return &node;
        }
        return nullptr;
    }

    bool IsOutputPin(UI::PinId pin)
    {
        for (auto& node : m_Nodes)
            for (auto& p : node.Outputs)
                if (p == pin) return true;
        return false;
    }

    void DeleteNode(UI::NodeId id)
    {
        auto it = m_Nodes.begin();
        while (it != m_Nodes.end())
        {
            if (it->Id == id)
            {
                for (auto& pin : it->Inputs)
                    RemoveLinksForPin(pin);
                for (auto& pin : it->Outputs)
                    RemoveLinksForPin(pin);
                it = m_Nodes.erase(it);
            }
            else
                ++it;
        }
    }

    void DeleteLink(UI::LinkId id)
    {
        m_Links.erase(std::remove_if(m_Links.begin(), m_Links.end(),
            [id](const Link& link) { return link.Id == id; }), m_Links.end());
    }

    void RemoveLinksForPin(UI::PinId pin)
    {
        m_Links.erase(std::remove_if(m_Links.begin(), m_Links.end(),
            [pin](const Link& link) { return link.StartPin == pin || link.EndPin == pin; }),
            m_Links.end());
    }

    std::unique_ptr<UI::NodeEditorContext> m_Context;
    std::vector<Node> m_Nodes;
    std::vector<Link> m_Links;
    int m_NextId = 1;
    UI::NodeId m_ContextNode;
    UI::LinkId m_ContextLink;
    glm::vec2 m_ContextMenuPos{0, 0};
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "ui/01-node-editor";
    auto* app = new Application(specs);
    app->PushLayer<NodeEditorLayer>();
    return app;
}
