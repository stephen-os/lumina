#include "Camera.h"

#include <algorithm>
#include <cmath>

namespace Lumina
{
    Camera::Camera()
    {
        UpdateViewMatrix();
    }

    void Camera::Translate(const glm::vec3& translation)
    {
        m_Position += translation;
        UpdateViewMatrix();
    }

    void Camera::TranslateLocal(const glm::vec3& translation)
    {
        glm::vec3 forward = GetForward();
        glm::vec3 right = GetRight();
        glm::vec3 up = GetUp();

        m_Position += right * translation.x + up * translation.y + forward * translation.z;
        UpdateViewMatrix();
    }

    void Camera::SetRotation(const glm::vec3& rotation)
    {
        m_Rotation = rotation;
        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::SetEulerAngles(float pitch, float yaw, float roll)
    {
        m_Rotation = glm::vec3(pitch, yaw, roll);
        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::Rotate(const glm::vec3& rotation)
    {
        m_Rotation += rotation;
        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::RotatePitch(float angle)
    {
        m_Rotation.x += angle;
        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::RotateYaw(float angle)
    {
        m_Rotation.y += angle;
        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::RotateRoll(float angle)
    {
        m_Rotation.z += angle;
        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::LookAt(const glm::vec3& target)
    {
        glm::vec3 direction = glm::normalize(m_Position - target);

        // Calculate yaw and pitch, preserve roll
        m_Rotation.y = glm::degrees(atan2(direction.x, direction.z));
        m_Rotation.x = glm::degrees(asin(-direction.y));
        // Keep existing roll (m_Rotation.z unchanged)

        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::LookAt(const glm::vec3& target, const glm::vec3& up)
    {
        glm::vec3 forward = glm::normalize(m_Position - target);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        glm::vec3 actualUp = glm::normalize(glm::cross(right, forward));

        // Calculate Euler angles from the look-at matrix
        m_Rotation.y = glm::degrees(atan2(forward.x, forward.z));  // Yaw
        m_Rotation.x = glm::degrees(asin(-forward.y));             // Pitch

        // Calculate roll from the up vector
        glm::vec3 expectedRight = glm::cross(forward, glm::vec3(0, 1, 0));
        if (glm::length(expectedRight) > 0.001f) {
            expectedRight = glm::normalize(expectedRight);
            m_Rotation.z = glm::degrees(atan2(glm::dot(actualUp, expectedRight), glm::dot(actualUp, glm::vec3(0, 1, 0))));
        }
        else {
            m_Rotation.z = 0.0f;
        }

        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::LookAtSmooth(const glm::vec3& target, float speed, float deltaTime)
    {
        glm::vec3 direction = glm::normalize(m_Position - target);

        float targetYaw = glm::degrees(atan2(direction.x, direction.z));
        float targetPitch = glm::degrees(asin(-direction.y));

        // Smooth interpolation
        m_Rotation.x = glm::mix(m_Rotation.x, targetPitch, speed * deltaTime);
        m_Rotation.y = glm::mix(m_Rotation.y, targetYaw, speed * deltaTime);
        // Keep existing roll

        ApplyConstraints();
        UpdateViewMatrix();
    }

    void Camera::LookAtSmooth(const glm::vec3& target, float speed, float deltaTime, const glm::vec3& up)
    {
        glm::vec3 forward = glm::normalize(m_Position - target);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        glm::vec3 actualUp = glm::normalize(glm::cross(right, forward));

        float targetYaw = glm::degrees(atan2(forward.x, forward.z));
        float targetPitch = glm::degrees(asin(-forward.y));
        float targetRoll = 0.0f;

        glm::vec3 expectedRight = glm::cross(forward, glm::vec3(0, 1, 0));
        if (glm::length(expectedRight) > 0.001f) {
            expectedRight = glm::normalize(expectedRight);
            targetRoll = glm::degrees(atan2(glm::dot(actualUp, expectedRight), glm::dot(actualUp, glm::vec3(0, 1, 0))));
        }

        // Smooth interpolation
        m_Rotation.x = glm::mix(m_Rotation.x, targetPitch, speed * deltaTime);
        m_Rotation.y = glm::mix(m_Rotation.y, targetYaw, speed * deltaTime);
        m_Rotation.z = glm::mix(m_Rotation.z, targetRoll, speed * deltaTime);

        ApplyConstraints();
        UpdateViewMatrix();
    }

    glm::vec3 Camera::GetForward() const
    {
        float pitch = glm::radians(m_Rotation.x);
        float yaw = glm::radians(m_Rotation.y);

        return glm::normalize(glm::vec3(
            cos(pitch) * sin(yaw),
            -sin(pitch),
            cos(pitch) * cos(yaw)
        ));
    }

    glm::vec3 Camera::GetRight() const
    {
        glm::vec3 forward = GetForward();
        return glm::normalize(glm::cross(forward, m_Up));
    }

    glm::vec3 Camera::GetUp() const
    {
        glm::vec3 forward = GetForward();
        glm::vec3 right = GetRight();
        return glm::normalize(glm::cross(right, forward));
    }

    void Camera::SetPitchConstraints(float minPitch, float maxPitch)
    {
        m_HasPitchConstraints = true;
        m_PitchConstraints = glm::vec2(minPitch, maxPitch);
        ApplyConstraints();
    }

    void Camera::SetYawConstraints(float minYaw, float maxYaw)
    {
        m_HasYawConstraints = true;
        m_YawConstraints = glm::vec2(minYaw, maxYaw);
        ApplyConstraints();
    }

    void Camera::SetRollConstraints(float minRoll, float maxRoll)
    {
        m_HasRollConstraints = true;
        m_RollConstraints = glm::vec2(minRoll, maxRoll);
        ApplyConstraints();
    }

    void Camera::ClearConstraints()
    {
        m_HasPitchConstraints = false;
        m_HasYawConstraints = false;
        m_HasRollConstraints = false;
    }

    void Camera::MoveForward(float distance)
    {
        m_Position += GetForward() * distance;
        UpdateViewMatrix();
    }

    void Camera::MoveRight(float distance)
    {
        m_Position += GetRight() * distance;
        UpdateViewMatrix();
    }

    void Camera::MoveUp(float distance)
    {
        m_Position += GetUp() * distance;
        UpdateViewMatrix();
    }

    glm::vec3 Camera::ScreenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize, float depth) const
    {
        // Convert screen coordinates to normalized device coordinates
        glm::vec4 ndc;
        ndc.x = (2.0f * screenPos.x) / screenSize.x - 1.0f;
        ndc.y = 1.0f - (2.0f * screenPos.y) / screenSize.y;
        ndc.z = depth;
        ndc.w = 1.0f;

        // Convert to world coordinates
        glm::mat4 invViewProj = glm::inverse(GetViewProjectionMatrix());
        glm::vec4 worldPos = invViewProj * ndc;

        return glm::vec3(worldPos) / worldPos.w;
    }

    glm::vec2 Camera::WorldToScreen(const glm::vec3& worldPos, const glm::vec2& screenSize) const
    {
        glm::vec4 clipPos = GetViewProjectionMatrix() * glm::vec4(worldPos, 1.0f);
        glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

        glm::vec2 screenPos;
        screenPos.x = (ndc.x + 1.0f) * 0.5f * screenSize.x;
        screenPos.y = (1.0f - ndc.y) * 0.5f * screenSize.y;

        return screenPos;
    }

    Camera::Ray Camera::GetRay(const glm::vec2& screenPos, const glm::vec2& screenSize) const
    {
        glm::vec3 nearPoint = ScreenToWorld(screenPos, screenSize, -1.0f);
        glm::vec3 farPoint = ScreenToWorld(screenPos, screenSize, 1.0f);

        Ray ray;
        ray.Origin = nearPoint;
        ray.Direction = glm::normalize(farPoint - nearPoint);

        return ray;
    }

    Camera::Frustum Camera::GetFrustum() const
    {
        Frustum frustum;
        glm::mat4 viewProj = GetViewProjectionMatrix();

        // Extract frustum planes from view-projection matrix
        // Left plane
        frustum.Left = glm::vec4(
            viewProj[0][3] + viewProj[0][0],
            viewProj[1][3] + viewProj[1][0],
            viewProj[2][3] + viewProj[2][0],
            viewProj[3][3] + viewProj[3][0]
        );

        // Right plane
        frustum.Right = glm::vec4(
            viewProj[0][3] - viewProj[0][0],
            viewProj[1][3] - viewProj[1][0],
            viewProj[2][3] - viewProj[2][0],
            viewProj[3][3] - viewProj[3][0]
        );

        // Bottom plane
        frustum.Bottom = glm::vec4(
            viewProj[0][3] + viewProj[0][1],
            viewProj[1][3] + viewProj[1][1],
            viewProj[2][3] + viewProj[2][1],
            viewProj[3][3] + viewProj[3][1]
        );

        // Top plane
        frustum.Top = glm::vec4(
            viewProj[0][3] - viewProj[0][1],
            viewProj[1][3] - viewProj[1][1],
            viewProj[2][3] - viewProj[2][1],
            viewProj[3][3] - viewProj[3][1]
        );

        // Near plane
        frustum.Near = glm::vec4(
            viewProj[0][3] + viewProj[0][2],
            viewProj[1][3] + viewProj[1][2],
            viewProj[2][3] + viewProj[2][2],
            viewProj[3][3] + viewProj[3][2]
        );

        // Far plane
        frustum.Far = glm::vec4(
            viewProj[0][3] - viewProj[0][2],
            viewProj[1][3] - viewProj[1][2],
            viewProj[2][3] - viewProj[2][2],
            viewProj[3][3] - viewProj[3][2]
        );

        return frustum;
    }

    bool Camera::IsPointInFrustum(const glm::vec3& point) const
    {
        Frustum frustum = GetFrustum();
        glm::vec4 testPoint = glm::vec4(point, 1.0f);

        return (glm::dot(frustum.Left, testPoint) >= 0.0f &&
            glm::dot(frustum.Right, testPoint) >= 0.0f &&
            glm::dot(frustum.Bottom, testPoint) >= 0.0f &&
            glm::dot(frustum.Top, testPoint) >= 0.0f &&
            glm::dot(frustum.Near, testPoint) >= 0.0f &&
            glm::dot(frustum.Far, testPoint) >= 0.0f);
    }

    bool Camera::IsSphereInFrustum(const glm::vec3& center, float radius) const
    {
        Frustum frustum = GetFrustum();
        glm::vec4 testPoint = glm::vec4(center, 1.0f);

        return (glm::dot(frustum.Left, testPoint) >= -radius &&
            glm::dot(frustum.Right, testPoint) >= -radius &&
            glm::dot(frustum.Bottom, testPoint) >= -radius &&
            glm::dot(frustum.Top, testPoint) >= -radius &&
            glm::dot(frustum.Near, testPoint) >= -radius &&
            glm::dot(frustum.Far, testPoint) >= -radius);
    }

    void Camera::SetTargetPosition(const glm::vec3& target)
    {
        m_TargetPosition = target;
        m_HasPositionTarget = true;
    }

    void Camera::SetTargetRotation(const glm::vec3& target)
    {
        m_TargetRotation = target;
        m_HasRotationTarget = true;
    }

    void Camera::UpdateSmooth(float deltaTime, float positionSpeed, float rotationSpeed)
    {
        if (m_HasPositionTarget) {
            m_Position = glm::mix(m_Position, m_TargetPosition, positionSpeed * deltaTime);

            // Check if close enough to target
            if (glm::distance(m_Position, m_TargetPosition) < 0.001f) {
                m_Position = m_TargetPosition;
                m_HasPositionTarget = false;
            }
        }

        if (m_HasRotationTarget) {
            m_Rotation = glm::mix(m_Rotation, m_TargetRotation, rotationSpeed * deltaTime);

            // Check if close enough to target
            if (glm::distance(m_Rotation, m_TargetRotation) < 0.1f) {
                m_Rotation = m_TargetRotation;
                m_HasRotationTarget = false;
            }

            ApplyConstraints();
        }

        if (m_HasPositionTarget || m_HasRotationTarget) {
            UpdateViewMatrix();
        }
    }

    void Camera::UpdateViewMatrix()
    {
        // Create rotation matrix from Euler angles (YXZ order)
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)); // Yaw
        rotation = glm::rotate(rotation, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)); // Pitch
        rotation = glm::rotate(rotation, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1)); // Roll

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_Position);
        m_ViewMatrix = glm::transpose(rotation) * translation;
    }

    void Camera::ApplyConstraints()
    {
        if (m_HasPitchConstraints) {
            m_Rotation.x = glm::clamp(m_Rotation.x, m_PitchConstraints.x, m_PitchConstraints.y);
        }

        if (m_HasYawConstraints) {
            m_Rotation.y = glm::clamp(m_Rotation.y, m_YawConstraints.x, m_YawConstraints.y);
        }

        if (m_HasRollConstraints) {
            m_Rotation.z = glm::clamp(m_Rotation.z, m_RollConstraints.x, m_RollConstraints.y);
        }
    }
}