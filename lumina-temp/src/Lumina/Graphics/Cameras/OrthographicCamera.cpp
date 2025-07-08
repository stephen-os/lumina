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

        // Update base dimensions and recalculate zoom
        m_BaseWidth = right - left;
        m_BaseHeight = top - bottom;
        m_Zoom = 1.0f; // Reset zoom when bounds are manually set

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetClippingPlanes(float nearPlane, float farPlane)
    {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetSize(float width, float height)
    {
        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;
        m_Left = -halfWidth;
        m_Right = halfWidth;
        m_Bottom = -halfHeight;
        m_Top = halfHeight;

        // Update base dimensions and reset zoom
        m_BaseWidth = width;
        m_BaseHeight = height;
        m_Zoom = 1.0f;

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetZoom(float zoom)
    {
        if (zoom <= 0.0f) zoom = 0.001f;

        m_Zoom = zoom;

        // Calculate new dimensions based on base size and zoom
        float newWidth = m_BaseWidth / zoom;
        float newHeight = m_BaseHeight / zoom;

        float halfWidth = newWidth / 2.0f;
        float halfHeight = newHeight / 2.0f;

        m_Left = -halfWidth;
        m_Right = halfWidth;
        m_Bottom = -halfHeight;
        m_Top = halfHeight;

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetOrthoParams(float left, float right, float bottom,
        float top, float nearPlane, float farPlane)
    {
        m_Left = left;
        m_Right = right;
        m_Bottom = bottom;
        m_Top = top;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;

        // Update base dimensions and reset zoom
        m_BaseWidth = right - left;
        m_BaseHeight = top - bottom;
        m_Zoom = 1.0f;

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::UpdateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearPlane, m_FarPlane);
    }
}