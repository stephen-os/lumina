// ui/01-node-editor: Node-based visual editor
// Demonstrates: Node editor, nodes, pins, links, context menus

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <algorithm>

namespace ui = lumina::ui;

// Simple node structure
struct Node
{
    ui::node_id id;
    std::string name;
    std::vector<ui::pin_id> inputs;
    std::vector<ui::pin_id> outputs;
    glm::vec2 initial_position{0, 0};
    bool position_set = false;
};

// Simple link structure
struct Link
{
    ui::link_id id;
    ui::pin_id start_pin;
    ui::pin_id end_pin;
};

class node_editor_layer : public lumina::core::layer
{
public:
    node_editor_layer() : layer("node_editor") {}

    void on_attach() override
    {
        // Create node editor context
        ui::ne_config config;
        config.SettingsFile = "ui-01-node-editor.json";
        m_context = std::make_unique<ui::node_editor_context>(&config);

        // Create some example nodes
        create_node("Start", 0, 1, {50, 100});
        create_node("Process A", 1, 1, {250, 50});
        create_node("Process B", 1, 1, {250, 200});
        create_node("Combine", 2, 1, {450, 125});
        create_node("Output", 1, 0, {650, 125});

        // Create some example links
        create_link(m_nodes[0].outputs[0], m_nodes[1].inputs[0]);
        create_link(m_nodes[0].outputs[0], m_nodes[2].inputs[0]);
        create_link(m_nodes[1].outputs[0], m_nodes[3].inputs[0]);
        create_link(m_nodes[2].outputs[0], m_nodes[3].inputs[1]);
        create_link(m_nodes[3].outputs[0], m_nodes[4].inputs[0]);
    }

    void on_detach() override
    {
        m_context.reset();
    }

    void on_render() override
    {
        ui::begin_window("Node Editor");
        ui::text("Right-click for context menu, drag to connect pins");
        ui::separator();

        m_context->set_current();
        ui::ne_begin("Node Editor", glm::vec2(0, 0));

        for (auto& node : m_nodes)
            draw_node(node);

        for (auto& link : m_links)
            ui::ne_link(link.id, link.start_pin, link.end_pin, glm::vec4(1, 1, 1, 1), 2.0f);

        // Handle link creation
        if (ui::ne_begin_create())
        {
            ui::pin_id start_pin, end_pin;
            if (ui::ne_query_new_link(&start_pin, &end_pin))
            {
                if (start_pin && end_pin && can_create_link(start_pin, end_pin))
                {
                    if (ui::ne_accept_new_item())
                        create_link(start_pin, end_pin);
                }
                else
                {
                    ui::ne_reject_new_item();
                }
            }
        }
        ui::ne_end_create();

        // Handle deletion
        if (ui::ne_begin_delete())
        {
            ui::link_id link_id;
            while (ui::ne_query_deleted_link(&link_id))
                if (ui::ne_accept_deleted_item())
                    delete_link(link_id);

            ui::node_id node_id;
            while (ui::ne_query_deleted_node(&node_id))
                if (ui::ne_accept_deleted_item())
                    delete_node(node_id);
        }
        ui::ne_end_delete();

        // Context menus
        ui::ne_suspend();
        handle_context_menus();
        ui::ne_resume();

        ui::ne_end();

        ui::separator();
        ui::text_fmt("Nodes: {}  Links: {}", m_nodes.size(), m_links.size());
        ui::text_fmt("Zoom: {:.2f}x", ui::ne_get_current_zoom());

        ui::end_window();
    }

private:
    void draw_node(Node& node)
    {
        if (!node.position_set)
        {
            ui::ne_set_node_position(node.id, node.initial_position);
            node.position_set = true;
        }

        ui::ne_begin_node(node.id);
        ImGui::Text("%s", node.name.c_str());

        for (size_t i = 0; i < node.inputs.size(); ++i)
        {
            ui::ne_begin_pin(node.inputs[i], ui::pin_kind::Input);
            ImGui::Text("-> In %zu", i);
            ui::ne_end_pin();
        }

        for (size_t i = 0; i < node.outputs.size(); ++i)
        {
            ui::ne_begin_pin(node.outputs[i], ui::pin_kind::Output);
            ImGui::Text("Out %zu ->", i);
            ui::ne_end_pin();
        }

        ui::ne_end_node();
    }

    void handle_context_menus()
    {
        ui::node_id context_node_id;
        ui::link_id context_link_id;

        if (ui::ne_show_node_context_menu(&context_node_id))
        {
            ImGui::OpenPopup("Node Context Menu");
            m_context_node = context_node_id;
        }
        else if (ui::ne_show_link_context_menu(&context_link_id))
        {
            ImGui::OpenPopup("Link Context Menu");
            m_context_link = context_link_id;
        }
        else if (ui::ne_show_background_context_menu())
        {
            auto mouse = ImGui::GetMousePos();
            m_context_menu_pos = {mouse.x, mouse.y};
            ImGui::OpenPopup("Background Context Menu");
        }

        if (ImGui::BeginPopup("Node Context Menu"))
        {
            if (ImGui::MenuItem("Delete Node"))
                delete_node(m_context_node);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Link Context Menu"))
        {
            if (ImGui::MenuItem("Delete Link"))
                delete_link(m_context_link);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Background Context Menu"))
        {
            glm::vec2 canvas_pos = ui::ne_screen_to_canvas(m_context_menu_pos);
            if (ImGui::MenuItem("Add Process Node"))
                create_node("Process", 1, 1, canvas_pos);
            if (ImGui::MenuItem("Add Input Node"))
                create_node("Input", 0, 1, canvas_pos);
            if (ImGui::MenuItem("Add Output Node"))
                create_node("Output", 1, 0, canvas_pos);
            ImGui::Separator();
            if (ImGui::MenuItem("Navigate to Content"))
                ui::ne_navigate_to_content();
            ImGui::EndPopup();
        }
    }

    void create_node(const std::string& name, int input_count, int output_count, glm::vec2 position)
    {
        Node node;
        node.id = ui::node_id(m_next_id++);
        node.name = name;
        for (int i = 0; i < input_count; ++i)
            node.inputs.push_back(ui::pin_id(m_next_id++));
        for (int i = 0; i < output_count; ++i)
            node.outputs.push_back(ui::pin_id(m_next_id++));
        node.initial_position = position;
        m_nodes.push_back(node);
    }

    void create_link(ui::pin_id start, ui::pin_id end)
    {
        Link link;
        link.id = ui::link_id(m_next_id++);
        link.start_pin = start;
        link.end_pin = end;
        m_links.push_back(link);
    }

    bool can_create_link(ui::pin_id start, ui::pin_id end)
    {
        Node* start_node = find_node_by_pin(start);
        Node* end_node = find_node_by_pin(end);
        if (!start_node || !end_node || start_node == end_node)
            return false;
        return is_output_pin(start) != is_output_pin(end);
    }

    Node* find_node_by_pin(ui::pin_id pin)
    {
        for (auto& node : m_nodes)
        {
            for (auto& p : node.inputs)
                if (p == pin) return &node;
            for (auto& p : node.outputs)
                if (p == pin) return &node;
        }
        return nullptr;
    }

    bool is_output_pin(ui::pin_id pin)
    {
        for (auto& node : m_nodes)
            for (auto& p : node.outputs)
                if (p == pin) return true;
        return false;
    }

    void delete_node(ui::node_id id)
    {
        auto it = m_nodes.begin();
        while (it != m_nodes.end())
        {
            if (it->id == id)
            {
                for (auto& pin : it->inputs)
                    remove_links_for_pin(pin);
                for (auto& pin : it->outputs)
                    remove_links_for_pin(pin);
                it = m_nodes.erase(it);
            }
            else
                ++it;
        }
    }

    void delete_link(ui::link_id id)
    {
        m_links.erase(std::remove_if(m_links.begin(), m_links.end(),
            [id](const Link& link) { return link.id == id; }), m_links.end());
    }

    void remove_links_for_pin(ui::pin_id pin)
    {
        m_links.erase(std::remove_if(m_links.begin(), m_links.end(),
            [pin](const Link& link) { return link.start_pin == pin || link.end_pin == pin; }),
            m_links.end());
    }

    std::unique_ptr<ui::node_editor_context> m_context;
    std::vector<Node> m_nodes;
    std::vector<Link> m_links;
    int m_next_id = 1;
    ui::node_id m_context_node;
    ui::link_id m_context_link;
    glm::vec2 m_context_menu_pos{0, 0};
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    application_specifications specs;
    specs.title = "ui/01-node-editor";
    auto* app = new application(specs);
    app->push_layer<node_editor_layer>();
    return app;
}
