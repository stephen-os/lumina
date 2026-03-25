#pragma once

#include <lumina/core/base.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <limits>

namespace Lumina
{
    /// Camera movement constraints.
    struct Camera2DBounds
    {
        float MinX = -std::numeric_limits<float>::max();
        float MaxX = std::numeric_limits<float>::max();
        float MinY = -std::numeric_limits<float>::max();
        float MaxY = std::numeric_limits<float>::max();

        [[nodiscard]] bool IsUnbounded() const noexcept
        {
            return MinX == -std::numeric_limits<float>::max() &&
                   MaxX == std::numeric_limits<float>::max() &&
                   MinY == -std::numeric_limits<float>::max() &&
                   MaxY == std::numeric_limits<float>::max();
        }

        [[nodiscard]] glm::vec2 Clamp(const glm::vec2& pos) const noexcept
        {
            return glm::vec2(
                std::clamp(pos.x, MinX, MaxX),
                std::clamp(pos.y, MinY, MaxY)
            );
        }
    };

    /// Camera shake configuration.
    struct Camera2DShakeParams
    {
        float Intensity = 1.0f;      // Maximum shake offset in world units
        float Frequency = 20.0f;     // Oscillation frequency
        float Decay = 5.0f;          // How fast shake diminishes
        float Duration = 0.0f;       // Total shake duration (0 = infinite until stopped)
    };

    /// 2D orthographic camera with follow, shake, and bounds support.
    /// Call update() once per frame to apply follow/shake behavior.
    class Camera2D
    {
    public:
        Camera2D();
        explicit Camera2D(float viewHeight, float aspectRatio = 16.0f / 9.0f);
        ~Camera2D() = default;

        /// Core transforms
        void SetPosition(const glm::vec2& pos);
        void SetPosition(float x, float y) { SetPosition({x, y}); }
        void Move(const glm::vec2& delta);
        void Move(float dx, float dy) { Move({dx, dy}); }

        void SetZoom(float zoom);
        [[nodiscard]] float GetZoom() const noexcept { return m_Zoom; }

        void SetRotation(float radians);
        [[nodiscard]] float GetRotation() const noexcept { return m_Rotation; }

        void SetViewHeight(float height);
        [[nodiscard]] float GetViewHeight() const noexcept { return m_ViewHeight; }

        void SetAspectRatio(float ratio);
        [[nodiscard]] float GetAspectRatio() const noexcept { return m_AspectRatio; }

        void SetNearFar(float nearPlane, float farPlane);

        [[nodiscard]] glm::vec2 GetPosition() const noexcept { return m_Position; }

        /// Follow/Lerp - camera smoothly follows a target position
        void SetTarget(const glm::vec2& target);
        void SetTarget(float x, float y) { SetTarget({x, y}); }
        void SetFollowSpeed(float speed);
        void SetFollowDeadzone(float radius);
        void ClearTarget();
        [[nodiscard]] bool HasTarget() const noexcept { return m_HasTarget; }
        [[nodiscard]] glm::vec2 GetTarget() const noexcept { return m_Target; }

        /// Shake - camera vibration effect
        void StartShake(const Camera2DShakeParams& params);
        void StartShake(float intensity, float duration = 0.5f);
        void StopShake();
        [[nodiscard]] bool IsShaking() const noexcept { return m_ShakeRemaining > 0.0f; }

        /// Bounds - constrain camera position
        void SetBounds(const Camera2DBounds& bounds);
        void SetBounds(float minX, float minY, float maxX, float maxY);
        void ClearBounds();
        [[nodiscard]] const Camera2DBounds& GetBounds() const noexcept { return m_Bounds; }

        /// Update (call once per frame)
        void Update(float deltaTime);

        /// Matrix access
        [[nodiscard]] const glm::mat4& GetViewMatrix() const noexcept { return m_ViewMatrix; }
        [[nodiscard]] const glm::mat4& GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; }
        [[nodiscard]] glm::mat4 GetViewProjectionMatrix() const;

        /// Coordinate conversion
        [[nodiscard]] glm::vec2 ScreenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize) const;
        [[nodiscard]] glm::vec2 WorldToScreen(const glm::vec2& worldPos, const glm::vec2& screenSize) const;

        /// Viewport info
        [[nodiscard]] float GetVisibleWidth() const;
        [[nodiscard]] float GetVisibleHeight() const;
        [[nodiscard]] glm::vec4 GetVisibleBounds() const;

    private:
        void UpdateViewMatrix();
        void UpdateProjectionMatrix();
        void ApplyShake(float deltaTime);
        void ApplyFollow(float deltaTime);
        void ApplyBounds();
        glm::vec2 CalculateShakeOffset() const;

        // Core state
        glm::vec2 m_Position{0.0f};
        float m_Zoom = 1.0f;
        float m_Rotation = 0.0f;
        float m_ViewHeight = 10.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        float m_NearPlane = -1000.0f;
        float m_FarPlane = 1000.0f;

        // Follow state
        bool m_HasTarget = false;
        glm::vec2 m_Target{0.0f};
        float m_FollowSpeed = 5.0f;
        float m_FollowDeadzone = 0.0f;

        // Shake state
        Camera2DShakeParams m_ShakeParams{};
        float m_ShakeRemaining = 0.0f;
        float m_ShakeTime = 0.0f;

        // Bounds
        Camera2DBounds m_Bounds{};

        // Cached matrices
        glm::mat4 m_ViewMatrix{1.0f};
        glm::mat4 m_ProjectionMatrix{1.0f};
        bool m_ViewDirty = true;
        bool m_ProjectionDirty = true;
    };
}
