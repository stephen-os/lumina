#pragma once

#include <glm/glm.hpp>

namespace Lumina
{
    struct Vertex3D
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec2 TexCoord = glm::vec2(0.0f);
        glm::vec3 Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 Bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
    };
}
