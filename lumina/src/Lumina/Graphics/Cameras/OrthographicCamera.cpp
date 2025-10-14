#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Lumina
{
    Ref<OrthographicCamera> OrthographicCamera::Create(float viewHeight, float nearPlane, float farPlane)
    {
        return CreateRef<OrthographicCamera>(viewHeight, nearPlane, farPlane);
    }

    OrthographicCamera::OrthographicCamera(float viewHeight, float nearPlane, float farPlane)
        : m_ViewHeight(viewHeight)
        , m_NearPlane(nearPlane)
        , m_FarPlane(farPlane)
    {
        RecalculateBounds();
        UpdateProjectionMatrix();
        UpdateViewMatrix();
    }

    void OrthographicCamera::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        UpdateViewMatrix();
    }

    void OrthographicCamera::SetViewHeight(float height)
    {
        m_ViewHeight = height;
        RecalculateBounds();
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetZoom(float zoom)
    {
        m_Zoom = std::max(zoom, 0.001f);
        RecalculateBounds();
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetAspectRatio(float aspectRatio)
    {
        m_AspectRatio = aspectRatio;
        RecalculateBounds();
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetClippingPlanes(float nearPlane, float farPlane)
    {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::RecalculateBounds()
    {
        float actualHeight = m_ViewHeight / m_Zoom;
        float actualWidth = actualHeight * m_AspectRatio;

        m_Left = -actualWidth * 0.5f;
        m_Right = actualWidth * 0.5f;
        m_Bottom = -actualHeight * 0.5f;
        m_Top = actualHeight * 0.5f;
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