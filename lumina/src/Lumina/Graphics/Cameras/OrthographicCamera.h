#pragma once
#include "Camera.h"
#include "Lumina/Core/Base.h"

namespace Lumina
{
    class OrthographicCamera : public Camera
    {
    public:
        static Ref<OrthographicCamera> Create(float left = -10.0f, float right = 10.0f, float bottom = -10.0f, float top = 10.0f, float nearPlane = -100.0f, float farPlane = 100.0f);

        OrthographicCamera(float left, float right, float bottom, float top, float nearPlane = -100.0f, float farPlane = 100.0f);
        virtual ~OrthographicCamera() = default;

        void SetPosition(const glm::vec3& position) override;
        void SetBounds(float left, float right, float bottom, float top);
        void SetClippingPlanes(float nearPlane, float farPlane);
        void SetViewportSize(float width, float height);
        void SetZoom(float zoom);

        float GetLeft() const { return m_Left; }
        float GetRight() const { return m_Right; }
        float GetBottom() const { return m_Bottom; }
        float GetTop() const { return m_Top; }
        float GetNearPlane() const { return m_NearPlane; }
        float GetFarPlane() const { return m_FarPlane; }
        float GetZoom() const { return m_Zoom; }
        float GetWidth() const { return m_Right - m_Left; }
        float GetHeight() const { return m_Top - m_Bottom; }

    protected:
        void UpdateViewMatrix() override;
        void UpdateProjectionMatrix() override;

    private:
        float m_Left;
        float m_Right;
        float m_Bottom;
        float m_Top;
        float m_NearPlane;
        float m_FarPlane;

        float m_Zoom = 1.0f;
        float m_BaseLeft;
        float m_BaseRight;
        float m_BaseBottom;
        float m_BaseTop;
    };
}