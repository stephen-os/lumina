#include "PerspectiveCamera.h"

namespace Lumina
{
    PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane)
    {
        UpdateProjectionMatrix();
    }

    void PerspectiveCamera::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        UpdateViewMatrix();
    }

    void PerspectiveCamera::SetFOV(float fov)
    {
        m_FOV = fov;
        UpdateProjectionMatrix();
    }

    void PerspectiveCamera::SetAspectRatio(float aspectRatio)
    {
        m_AspectRatio = aspectRatio;
        UpdateProjectionMatrix();
    }

    void PerspectiveCamera::SetClippingPlanes(float nearPlane, float farPlane)
    {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }

    void PerspectiveCamera::SetPerspectiveParams(float fov, float aspectRatio, float nearPlane, float farPlane)
    {
        m_FOV = fov;
        m_AspectRatio = aspectRatio;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }

    void PerspectiveCamera::UpdateProjectionMatrix()
    {
        // m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
        m_ProjectionMatrix[1][1] *= -1.0f;
        UpdateViewMatrix(); 
    }
}