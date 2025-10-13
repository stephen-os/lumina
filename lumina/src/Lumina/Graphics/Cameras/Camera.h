#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace Lumina
{

    class Camera
    {
    public:
        struct Ray
        {
            glm::vec3 Origin;
            glm::vec3 Direction;
        };

        struct Frustum
        {
            glm::vec4 Left;
            glm::vec4 Right;
            glm::vec4 Bottom;
            glm::vec4 Top;
            glm::vec4 Near;
            glm::vec4 Far;
        };

    public:
        Camera();
        virtual ~Camera() = default;

        virtual void SetPosition(const glm::vec3& position) = 0;
		void Translate(const glm::vec3& translation);
        void TranslateLocal(const glm::vec3& translation); 

        void SetRotation(const glm::vec3& rotation);
        void SetEulerAngles(float pitch, float yaw, float roll); 
        void Rotate(const glm::vec3& rotation);
        void RotatePitch(float angle);
        void RotateYaw(float angle);
        void RotateRoll(float angle);

        void LookAt(const glm::vec3& target);
        void LookAt(const glm::vec3& target, const glm::vec3& up);
        void LookAtSmooth(const glm::vec3& target, float speed, float deltaTime);
        void LookAtSmooth(const glm::vec3& target, float speed, float deltaTime, const glm::vec3& up);

		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetRotation() const { return m_Rotation; }

        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;
        float GetPitch() const { return m_Rotation.x; }
        float GetYaw() const { return m_Rotation.y; }
        float GetRoll() const { return m_Rotation.z; }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; };
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; };
        glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }
        
        void SetPitchConstraints(float minPitch, float maxPitch);
        void SetYawConstraints(float minYaw, float maxYaw);
        void SetRollConstraints(float minRoll, float maxRoll);
        void ClearConstraints();

        // Movement helpers
        void MoveForward(float distance);
        void MoveRight(float distance);
        void MoveUp(float distance);

        glm::vec3 ScreenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize, float depth = 0.0f) const;
        glm::vec2 WorldToScreen(const glm::vec3& worldPos, const glm::vec2& screenSize) const;

        Ray GetRay(const glm::vec2& screenPos, const glm::vec2& screenSize) const;

        Frustum GetFrustum() const;
        bool IsPointInFrustum(const glm::vec3& point) const;
        bool IsSphereInFrustum(const glm::vec3& center, float radius) const;

        void SetTargetPosition(const glm::vec3& target);
        void SetTargetRotation(const glm::vec3& target);
        void UpdateSmooth(float deltaTime, float positionSpeed = 5.0f, float rotationSpeed = 5.0f);

    protected: 
        void UpdateViewMatrix();
        virtual void UpdateProjectionMatrix() = 0;

        void ApplyConstraints();

    protected:
        // Core 
        glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Matrices
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        
        // Constraints
        bool m_HasPitchConstraints = false;
        bool m_HasYawConstraints = false;
        bool m_HasRollConstraints = false;
        glm::vec2 m_PitchConstraints = glm::vec2(-90.0f, 90.0f);
        glm::vec2 m_YawConstraints = glm::vec2(-180.0f, 180.0f);
        glm::vec2 m_RollConstraints = glm::vec2(-180.0f, 180.0f);

        // Smooth animation targets
        bool m_HasPositionTarget = false;
        bool m_HasRotationTarget = false;
        glm::vec3 m_TargetPosition = glm::vec3(0.0f);
        glm::vec3 m_TargetRotation = glm::vec3(0.0f);
	};
}