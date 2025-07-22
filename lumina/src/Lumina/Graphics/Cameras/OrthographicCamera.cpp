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
        // Store the old position
        glm::vec3 oldPosition = m_Position;

        // Set the new position
        m_Position = position;

        // Calculate how much the camera moved
        glm::vec3 delta = position - oldPosition;

        // Move the orthographic bounds by the same amount
        m_Left += delta.x;
        m_Right += delta.x;
        m_Bottom += delta.y;
        m_Top += delta.y;

        // Update view matrix (call parent implementation)
        m_ViewMatrix = glm::mat4(1.0f);

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::SetSize(float width, float height)
    {
        // Update base dimensions
        m_BaseWidth = width;
        m_BaseHeight = height;

        // Recalculate bounds based on current zoom and new base size
        float newWidth = m_BaseWidth / m_Zoom;
        float newHeight = m_BaseHeight / m_Zoom;
        float halfWidth = newWidth / 2.0f;
        float halfHeight = newHeight / 2.0f;

        // PRESERVE the current camera center position
        glm::vec3 currentCenter = GetPosition();

        m_Left = currentCenter.x - halfWidth;
        m_Right = currentCenter.x + halfWidth;
        m_Bottom = currentCenter.y - halfHeight;
        m_Top = currentCenter.y + halfHeight;

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

        // PRESERVE the current camera center position
        glm::vec3 currentCenter = GetPosition(); // Get current camera position

        // Set bounds around the current center, not world origin
        m_Left = currentCenter.x - halfWidth;
        m_Right = currentCenter.x + halfWidth;
        m_Bottom = currentCenter.y - halfHeight;
        m_Top = currentCenter.y + halfHeight;

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
        
        float currentZoom = m_Zoom;
        SetZoom(currentZoom);

        UpdateProjectionMatrix();
    }

    void OrthographicCamera::UpdateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearPlane, m_FarPlane);
    }
}