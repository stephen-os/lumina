#include "Camera.h"

#include <sstream>
#include <iomanip>
#include <iostream>

#include <imgui.h>
#include <GLFW/glfw3.h>

namespace Lumina
{
    Camera::Camera() { UpdateViewMatrix(); }

    void Camera::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        UpdateViewMatrix();
    }

	void Camera::Translate(const glm::vec3& translation)
	{
		SetPosition(m_Position + translation);
	}

    void Camera::SetRotation(const glm::quat& rotation)
    {
		m_Rotation = glm::normalize(rotation);
        UpdateViewMatrix();
    }

	void Camera::Rotate(float angle, const glm::vec3& axis)
	{
		m_Rotation = glm::angleAxis(glm::radians(angle), axis) * m_Rotation;
		UpdateViewMatrix();
	}

	void Camera::SetEulerAngles(float pitch, float yaw, float roll)
	{
		m_Rotation = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));
		UpdateViewMatrix();
	}

	void Camera::LookAt(const glm::vec3& target, const glm::vec3& up)
	{
        glm::vec3 forward = glm::normalize(target - m_Position);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        glm::vec3 actualUp = glm::cross(right, forward);

        glm::mat3 rotation;
        rotation[0] = right;
        rotation[1] = actualUp;
        rotation[2] = -forward;

        m_Rotation = glm::quat_cast(rotation);
        UpdateViewMatrix();
	}

    void Camera::UpdateViewMatrix()
    {
        glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(m_Rotation));
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -m_Position);
        m_ViewMatrix = rotationMatrix * translationMatrix;
    }

    std::string Camera::MatrixToString(const glm::mat4& matrix)
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);

        ss << "|-----------------------------|\n";

        for (int i = 0; i < 4; ++i) {
            ss << "| ";
            for (int j = 0; j < 4; ++j) {
                float value = matrix[j][i];
                ss << std::setw(7) << (value < 0 ? "-" : " ") << std::abs(value) << " ";
            }
            ss << "|\n";
        }

        ss << "|-----------------------------|\n";
        return ss.str();
    }
}
