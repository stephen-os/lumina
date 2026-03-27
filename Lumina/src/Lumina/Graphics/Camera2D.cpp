#include "Camera2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <algorithm>

namespace Lumina
{
    Camera2D::Camera2D()
    {
        UpdateProjectionMatrix();
        UpdateViewMatrix();
    }

    Camera2D::Camera2D(float viewHeight, float aspectRatio)
        : m_ViewHeight(viewHeight)
        , m_AspectRatio(aspectRatio)
    {
        UpdateProjectionMatrix();
        UpdateViewMatrix();
    }

    void Camera2D::SetPosition(const glm::vec2& pos)
    {
        m_Position = pos;
        ApplyBounds();
        m_ViewDirty = true;
    }

    void Camera2D::Move(const glm::vec2& delta)
    {
        m_Position += delta;
        ApplyBounds();
        m_ViewDirty = true;
    }

    void Camera2D::SetZoom(float zoom)
    {
        m_Zoom = std::max(0.001f, zoom);
        m_ProjectionDirty = true;
    }

    void Camera2D::SetRotation(float radians)
    {
        m_Rotation = radians;
        m_ViewDirty = true;
    }

    void Camera2D::SetViewHeight(float height)
    {
        m_ViewHeight = std::max(0.001f, height);
        m_ProjectionDirty = true;
    }

    void Camera2D::SetAspectRatio(float ratio)
    {
        m_AspectRatio = std::max(0.001f, ratio);
        m_ProjectionDirty = true;
    }

    void Camera2D::SetNearFar(float nearPlane, float farPlane)
    {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        m_ProjectionDirty = true;
    }

    void Camera2D::SetTarget(const glm::vec2& target)
    {
        m_Target = target;
        m_HasTarget = true;
    }

    void Camera2D::SetFollowSpeed(float speed)
    {
        m_FollowSpeed = std::max(0.0f, speed);
    }

    void Camera2D::SetFollowDeadzone(float radius)
    {
        m_FollowDeadzone = std::max(0.0f, radius);
    }

    void Camera2D::ClearTarget()
    {
        m_HasTarget = false;
    }

    void Camera2D::StartShake(const Camera2DShakeParams& params)
    {
        m_ShakeParams = params;
        m_ShakeRemaining = params.Duration > 0.0f ? params.Duration : std::numeric_limits<float>::max();
        m_ShakeTime = 0.0f;
    }

    void Camera2D::StartShake(float intensity, float duration)
    {
        Camera2DShakeParams params;
        params.Intensity = intensity;
        params.Duration = duration;
        params.Frequency = 20.0f;
        params.Decay = 5.0f;
        StartShake(params);
    }

    void Camera2D::StopShake()
    {
        m_ShakeRemaining = 0.0f;
        m_ShakeTime = 0.0f;
        m_ViewDirty = true;
    }

    void Camera2D::SetBounds(const Camera2DBounds& bounds)
    {
        m_Bounds = bounds;
        ApplyBounds();
        m_ViewDirty = true;
    }

    void Camera2D::SetBounds(float minX, float minY, float maxX, float maxY)
    {
        m_Bounds.MinX = minX;
        m_Bounds.MinY = minY;
        m_Bounds.MaxX = maxX;
        m_Bounds.MaxY = maxY;
        ApplyBounds();
        m_ViewDirty = true;
    }

    void Camera2D::ClearBounds()
    {
        m_Bounds = Camera2DBounds{};
    }

    void Camera2D::Update(float deltaTime)
    {
        ApplyFollow(deltaTime);
        ApplyShake(deltaTime);
        ApplyBounds();

        if (m_ViewDirty)
        {
            UpdateViewMatrix();
            m_ViewDirty = false;
        }
        if (m_ProjectionDirty)
        {
            UpdateProjectionMatrix();
            m_ProjectionDirty = false;
        }
    }

    glm::mat4 Camera2D::GetViewProjectionMatrix() const
    {
        return m_ProjectionMatrix * m_ViewMatrix;
    }

    glm::vec2 Camera2D::ScreenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize) const
    {
        // Convert screen position to NDC (-1 to 1)
        glm::vec2 ndc;
        ndc.x = (screenPos.x / screenSize.x) * 2.0f - 1.0f;
        ndc.y = 1.0f - (screenPos.y / screenSize.y) * 2.0f;  // Flip Y

        // Transform by inverse view-projection
        glm::mat4 invVp = glm::inverse(GetViewProjectionMatrix());
        glm::vec4 worldPos = invVp * glm::vec4(ndc.x, ndc.y, 0.0f, 1.0f);

        return glm::vec2(worldPos.x, worldPos.y);
    }

    glm::vec2 Camera2D::WorldToScreen(const glm::vec2& worldPos, const glm::vec2& screenSize) const
    {
        // Transform to clip space
        glm::vec4 clipPos = GetViewProjectionMatrix() * glm::vec4(worldPos.x, worldPos.y, 0.0f, 1.0f);

        // NDC
        glm::vec2 ndc = glm::vec2(clipPos.x, clipPos.y) / clipPos.w;

        // Screen coordinates
        glm::vec2 screen;
        screen.x = (ndc.x + 1.0f) * 0.5f * screenSize.x;
        screen.y = (1.0f - ndc.y) * 0.5f * screenSize.y;  // Flip Y

        return screen;
    }

    float Camera2D::GetVisibleWidth() const
    {
        return (m_ViewHeight / m_Zoom) * m_AspectRatio;
    }

    float Camera2D::GetVisibleHeight() const
    {
        return m_ViewHeight / m_Zoom;
    }

    glm::vec4 Camera2D::GetVisibleBounds() const
    {
        float width = GetVisibleWidth();
        float height = GetVisibleHeight();
        return glm::vec4(m_Position.x, m_Position.y, width, height);
    }

    void Camera2D::UpdateViewMatrix()
    {
        glm::vec2 finalPos = m_Position + CalculateShakeOffset();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f),
            glm::vec3(finalPos.x, finalPos.y, 0.0f));

        if (std::abs(m_Rotation) > 0.0001f)
        {
            transform = glm::rotate(transform, m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        m_ViewMatrix = glm::inverse(transform);
    }

    void Camera2D::UpdateProjectionMatrix()
    {
        float height = m_ViewHeight / m_Zoom;
        float width = height * m_AspectRatio;

        m_ProjectionMatrix = glm::ortho(
            -width * 0.5f, width * 0.5f,
            -height * 0.5f, height * 0.5f,
            m_NearPlane, m_FarPlane
        );
    }

    void Camera2D::ApplyShake(float deltaTime)
    {
        if (m_ShakeRemaining <= 0.0f)
            return;

        m_ShakeTime += deltaTime;
        m_ShakeRemaining -= deltaTime;

        if (m_ShakeRemaining <= 0.0f)
        {
            m_ShakeRemaining = 0.0f;
            m_ShakeTime = 0.0f;
        }

        m_ViewDirty = true;
    }

    void Camera2D::ApplyFollow(float deltaTime)
    {
        if (!m_HasTarget)
            return;

        glm::vec2 diff = m_Target - m_Position;
        float distance = glm::length(diff);

        // Check deadzone
        if (distance <= m_FollowDeadzone)
            return;

        // Smooth interpolation toward target
        float t = 1.0f - std::exp(-m_FollowSpeed * deltaTime);
        m_Position = glm::mix(m_Position, m_Target, t);

        m_ViewDirty = true;
    }

    void Camera2D::ApplyBounds()
    {
        if (m_Bounds.IsUnbounded())
            return;

        m_Position = m_Bounds.Clamp(m_Position);
    }

    glm::vec2 Camera2D::CalculateShakeOffset() const
    {
        if (m_ShakeRemaining <= 0.0f)
            return glm::vec2(0.0f);

        // Calculate decay factor
        float decayFactor = 1.0f;
        if (m_ShakeParams.Duration > 0.0f)
        {
            float elapsed = m_ShakeParams.Duration - m_ShakeRemaining;
            decayFactor = std::exp(-m_ShakeParams.Decay * elapsed);
        }

        // Use sin/cos with different frequencies for x/y to avoid circular patterns
        float time = m_ShakeTime * m_ShakeParams.Frequency;
        float offsetX = std::sin(time * 1.0f) * std::cos(time * 0.7f);
        float offsetY = std::sin(time * 1.3f) * std::cos(time * 0.9f);

        return glm::vec2(offsetX, offsetY) * m_ShakeParams.Intensity * decayFactor;
    }
}
