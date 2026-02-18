#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <string>
#include <vector>

namespace lumina::physics
{
    class world;

    // Debug draw configuration
    struct debug_draw_flags
    {
        bool draw_shapes = true;
        bool draw_joints = true;
        bool draw_aabbs = false;
        bool draw_mass = false;
        bool draw_contacts = false;
        bool draw_contact_normals = false;
    };

    // Abstract interface for debug rendering
    class debug_draw_interface
    {
    public:
        virtual ~debug_draw_interface() = default;

        virtual void draw_polygon(const glm::vec2* vertices, int vertex_count, const glm::vec4& color) = 0;
        virtual void draw_solid_polygon(const glm::vec2* vertices, int vertex_count, const glm::vec4& color) = 0;
        virtual void draw_circle(glm::vec2 center, float radius, const glm::vec4& color) = 0;
        virtual void draw_solid_circle(glm::vec2 center, float radius, glm::vec2 axis, const glm::vec4& color) = 0;
        virtual void draw_capsule(glm::vec2 p1, glm::vec2 p2, float radius, const glm::vec4& color) = 0;
        virtual void draw_solid_capsule(glm::vec2 p1, glm::vec2 p2, float radius, const glm::vec4& color) = 0;
        virtual void draw_segment(glm::vec2 p1, glm::vec2 p2, const glm::vec4& color) = 0;
        virtual void draw_point(glm::vec2 p, float size, const glm::vec4& color) = 0;
        virtual void draw_string(glm::vec2 p, const std::string& text, const glm::vec4& color) = 0;
    };

    // Helper to render world debug visualization
    void debug_draw_world(const world& w, debug_draw_interface& drawer, const debug_draw_flags& flags = {});
}
