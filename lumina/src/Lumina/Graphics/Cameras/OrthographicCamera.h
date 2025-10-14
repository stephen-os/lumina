#pragma once

#include "Camera.h"

#include "Lumina/Core/Base.h"

namespace Lumina
{
    class OrthographicCamera : public Camera
    {
    public:
        static Ref<OrthographicCamera> Create(float viewHeight = 8.0f, float nearPlane = -100.0f, float farPlane = 100.0f);
        
        OrthographicCamera(float viewHeight = 8.0f, float nearPlane = -100.0f, float farPlane = 100.0f);
        virtual ~OrthographicCamera() = default;

        void SetPosition(const glm::vec3& position) override;

        void SetViewHeight(float height);
        void SetZoom(float zoom);
        void SetAspectRatio(float aspectRatio);
        void SetClippingPlanes(float nearPlane, float farPlane);

        float GetViewHeight() const { return m_ViewHeight; }
        float GetZoom() const { return m_Zoom; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetLeft() const { return m_Left; }
        float GetRight() const { return m_Right; }
        float GetBottom() const { return m_Bottom; }
        float GetTop() const { return m_Top; }
        float GetNearPlane() const { return m_NearPlane; }
        float GetFarPlane() const { return m_FarPlane; }
        float GetWidth() const { return m_Right - m_Left; }
        float GetHeight() const { return m_Top - m_Bottom; }

    protected:
        void UpdateViewMatrix() override;
        void UpdateProjectionMatrix() override;
        void RecalculateBounds();

    private:
        float m_ViewHeight = 8.0f;
        float m_Zoom = 1.0f;
        float m_AspectRatio = 1.0f;
        float m_NearPlane = -100.0f;
        float m_FarPlane = 100.0f;

        float m_Left;
        float m_Right;
        float m_Bottom;
        float m_Top;
    };
}