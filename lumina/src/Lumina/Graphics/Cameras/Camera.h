#pragma once

#include <glm/glm.hpp>

namespace Lumina
{
    class Camera
    {
    public:
        Camera() = default;
        virtual ~Camera() = default;

        virtual void SetPosition(const glm::vec3& position) = 0;

        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

    protected:
        virtual void UpdateViewMatrix() = 0;
        virtual void UpdateProjectionMatrix() = 0;

    protected:
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    };
}