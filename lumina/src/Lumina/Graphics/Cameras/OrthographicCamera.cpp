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
		
        m_BaseLeft = left;
		m_BaseRight = right;
		m_BaseBottom = bottom;
		m_BaseTop = top;

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
        m_BaseLeft = left;
        m_BaseRight = right;
        m_BaseBottom = bottom;
        m_BaseTop = top;
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetClippingPlanes(float nearPlane, float farPlane)
    {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetViewportSize(float width, float height)
    {
        float aspectRatio = width / height;

        float baseHeight = m_Top - m_Bottom;
        float viewHeight = baseHeight / m_Zoom;
        float viewWidth = viewHeight * aspectRatio;

        m_Left = -viewWidth * 0.5f;
        m_Right = viewWidth * 0.5f;
        m_Bottom = -viewHeight * 0.5f;
        m_Top = viewHeight * 0.5f;

        m_BaseLeft = m_Left * m_Zoom;
        m_BaseRight = m_Right * m_Zoom;
        m_BaseBottom = m_Bottom * m_Zoom;
        m_BaseTop = m_Top * m_Zoom;

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetZoom(float zoom)
    {
        m_Zoom = std::max(zoom, 0.001f);

        float width = (m_BaseRight - m_BaseLeft) / m_Zoom;
        float height = (m_BaseTop - m_BaseBottom) / m_Zoom;

        float centerX = (m_BaseLeft + m_BaseRight) * 0.5f;
        float centerY = (m_BaseBottom + m_BaseTop) * 0.5f;

        m_Left = centerX - width * 0.5f;
        m_Right = centerX + width * 0.5f;
        m_Bottom = centerY - height * 0.5f;
        m_Top = centerY + height * 0.5f;

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