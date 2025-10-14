#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Lumina
{
    Ref<OrthographicCamera> OrthographicCamera::Create(float left, float right, float bottom, float top, float nearPlane, float farPlane)
    {
        return CreateRef<OrthographicCamera>(left, right, bottom, top, nearPlane, farPlane);
    }

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
    {
        m_Left = left;
		m_Right = right;
		m_Bottom = bottom;
		m_Top = top;
		m_NearPlane = nearPlane;
        m_FarPlane = farPlane; 

        UpdateProjectionMatrix();
        UpdateViewMatrix();
    }

    void OrthographicCamera::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        UpdateViewMatrix();
    }

    void OrthographicCamera::SetBounds(float left, float right, float bottom, float top)
    {
        m_Left = left;
        m_Right = right;
        m_Bottom = bottom;
        m_Top = top;

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetClippingPlanes(float nearPlane, float farPlane)
    {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::UpdateViewMatrix()
    {
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    }

    void OrthographicCamera::UpdateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearPlane, m_FarPlane);
    }
}