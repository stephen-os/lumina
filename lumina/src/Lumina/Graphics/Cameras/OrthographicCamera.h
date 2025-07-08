#include "Camera.h"

namespace Lumina
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left = -10.0f, float right = 10.0f, float bottom = -10.0f, float top = 10.0f, float nearPlane = -100.0f, float farPlane = 100.0f);
        virtual ~OrthographicCamera() = default;

        static OrthographicCamera Create2D(float width, float height);

        void SetBounds(float left, float right, float bottom, float top);
        void SetClippingPlanes(float nearPlane, float farPlane);
        void SetSize(float width, float height);
        void SetZoom(float zoom);
        void SetOrthoParams(float left, float right, float bottom, float top, float nearPlane, float farPlane);

        float GetLeft() const { return m_Left; }
        float GetRight() const { return m_Right; }
        float GetBottom() const { return m_Bottom; }
        float GetTop() const { return m_Top; }
        float GetNearPlane() const { return m_NearPlane; }
        float GetFarPlane() const { return m_FarPlane; }
        float GetWidth() const { return m_Right - m_Left; }
        float GetHeight() const { return m_Top - m_Bottom; }
		float GetZoom() const { return m_Zoom; }

    protected:
        void UpdateProjectionMatrix() override;

    private:
        float m_Left;
        float m_Right;
        float m_Bottom;
        float m_Top;
        float m_NearPlane;
        float m_FarPlane;
        
        float m_Zoom;
        float m_BaseWidth;
        float m_BaseHeight;
    };
}