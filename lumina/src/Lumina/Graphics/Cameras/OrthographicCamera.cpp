#include "OrthographicCamera.h"

namespace Lumina
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
        : m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top), m_NearPlane(nearPlane), m_FarPlane(farPlane)
    {
        m_BaseWidth = right - left;
        m_BaseHeight = top - bottom;
        m_Zoom = 1.0f;
        UpdateProjectionMatrix();
    }

    OrthographicCamera OrthographicCamera::Create2D(float width, float height)
    {
        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;
        return OrthographicCamera(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
    }

    void OrthographicCamera::SetBounds(float left, float right, float bottom, float top)
    {
        m_Left = left;
        m_Right = right;
        m_Bottom = bottom;
        m_Top = top;

        m_BaseWidth = right - left;
        m_BaseHeight = top - bottom;

        float currentZoom = m_Zoom;
        SetZoom(currentZoom);

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetClippingPlanes(float nearPlane, float farPlane)
    {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetPosition(const glm::vec3& position)
    {
        glm::vec3 delta = position - m_Position;
        m_Position = position; 

        m_Left += delta.x;
        m_Right += delta.x;
        m_Bottom += delta.y;
        m_Top += delta.y;

        UpdateViewMatrix(); 
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetSize(float width, float height)
    {
        m_BaseWidth = width;
        m_BaseHeight = height;

        float newWidth = m_BaseWidth / m_Zoom;
        float newHeight = m_BaseHeight / m_Zoom;
        float halfWidth = newWidth / 2.0f;
        float halfHeight = newHeight / 2.0f;

        glm::vec3 center = GetPosition();

        m_Left = center.x - halfWidth;
        m_Right = center.x + halfWidth;
        m_Bottom = center.y - halfHeight;
        m_Top = center.y + halfHeight;

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetZoom(float zoom)
    {
        if (zoom <= 0.0f) 
            zoom = 0.001f;
        m_Zoom = zoom;

        float newWidth = m_BaseWidth / zoom;
        float newHeight = m_BaseHeight / zoom;
        float halfWidth = newWidth / 2.0f;
        float halfHeight = newHeight / 2.0f;

        glm::vec3 center = GetPosition();

        m_Left = center.x - halfWidth;
        m_Right = center.x + halfWidth;
        m_Bottom = center.y - halfHeight;
        m_Top = center.y + halfHeight;

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetOrthoParams(float left, float right, float bottom, float top, float nearPlane, float farPlane)
    {
        m_Left = left;
        m_Right = right;
        m_Bottom = bottom;
        m_Top = top;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;

        m_BaseWidth = right - left;
        m_BaseHeight = top - bottom;
        
        float currentZoom = m_Zoom;
        SetZoom(currentZoom);

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::UpdateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearPlane, m_FarPlane);
    }
}