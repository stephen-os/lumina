#pragma once

#include <lumina/core/Base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <string>
#include <vector>

namespace Lumina
{
    class World;

    // Debug draw configuration
    struct DebugDrawFlags
    {
        bool DrawShapes = true;
        bool DrawJoints = true;
        bool DrawAABBs = false;
        bool DrawMass = false;
        bool DrawContacts = false;
        bool DrawContactNormals = false;
    };

    // Abstract interface for debug rendering
    class DebugDrawInterface
    {
    public:
        virtual ~DebugDrawInterface() = default;

        virtual void DrawPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) = 0;
        virtual void DrawSolidPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) = 0;
        virtual void DrawCircle(glm::vec2 center, float radius, const glm::vec4& color) = 0;
        virtual void DrawSolidCircle(glm::vec2 center, float radius, glm::vec2 axis, const glm::vec4& color) = 0;
        virtual void DrawCapsule(glm::vec2 p1, glm::vec2 p2, float radius, const glm::vec4& color) = 0;
        virtual void DrawSolidCapsule(glm::vec2 p1, glm::vec2 p2, float radius, const glm::vec4& color) = 0;
        virtual void DrawSegment(glm::vec2 p1, glm::vec2 p2, const glm::vec4& color) = 0;
        virtual void DrawPoint(glm::vec2 p, float size, const glm::vec4& color) = 0;
        virtual void DrawString(glm::vec2 p, const std::string& text, const glm::vec4& color) = 0;
    };

    // Helper to render world debug visualization
    void DebugDrawWorld(const World& w, DebugDrawInterface& drawer, const DebugDrawFlags& flags = {});
}
