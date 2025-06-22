// Camera.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace Lumina
{
    class Camera
    {
    public:
        Camera();
        virtual ~Camera() = default;

        void SetPosition(const glm::vec3& position);
		void Translate(const glm::vec3& translation);
        void SetRotation(const glm::quat& rotation);

        void Rotate(float angle, const glm::vec3& axis);
        void SetEulerAngles(float pitch, float yaw, float roll); 

        void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));


		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::quat& GetRotation() const { return m_Rotation; }

        glm::vec3 GetForward() const { return glm::normalize(m_Rotation * glm::vec3(0.0f, 0.0f, -1.0f)); };
        glm::vec3 GetRight() const { return glm::normalize(m_Rotation * glm::vec3(1.0f, 0.0f, 0.0f)); };
        glm::vec3 GetUp() const { return glm::normalize(m_Rotation * glm::vec3(0.0f, 1.0f, 0.0f)); };

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; };
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; };
      
        static std::string MatrixToString(const glm::mat4& matrix);

    protected: 
        void UpdateViewMatrix();
        virtual void UpdateProjectionMatrix() = 0;

    protected:

        glm::vec3 m_Position = glm::vec3(0.0f);
		glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
	};
}