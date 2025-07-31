#pragma once

#include "Camera.h"

namespace Lumina
{
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 100.0f);
        virtual ~PerspectiveCamera() = default;

		void SetPosition(const glm::vec3& position) override;
        void SetFOV(float fov);
        void SetAspectRatio(float aspectRatio);
        void SetClippingPlanes(float nearPlane, float farPlane);
        void SetPerspectiveParams(float fov, float aspectRatio, float nearPlane, float farPlane);

        float GetFOV() const { return m_FOV; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetNearPlane() const { return m_NearPlane; }
        float GetFarPlane() const { return m_FarPlane; }

    protected:
        void UpdateProjectionMatrix() override;

    private:
        float m_FOV;
        float m_AspectRatio;
        float m_NearPlane;
        float m_FarPlane;
    };
}