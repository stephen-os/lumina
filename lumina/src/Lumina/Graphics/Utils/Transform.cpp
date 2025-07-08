#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform()
    : m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f, 0.0f, 0.0f),
    m_Scale(1.0f, 1.0f, 1.0f), m_Matrix(glm::mat4(1.0f))
{
    UpdateMatrix();
}

Transform::Transform(const Transform& other)
    : m_Position(other.m_Position), m_Rotation(other.m_Rotation),
    m_Scale(other.m_Scale), m_Matrix(other.m_Matrix)
{
    UpdateMatrix();
}

Transform& Transform::operator=(const Transform& other)
{
    if (this != &other)
    {
        m_Position = other.m_Position;
        m_Rotation = other.m_Rotation;
        m_Scale = other.m_Scale;
        m_Matrix = other.m_Matrix;
        UpdateMatrix();
    }
    return *this;
}

Transform::Transform(Transform&& other) noexcept
    : m_Position(std::move(other.m_Position)),
    m_Rotation(std::move(other.m_Rotation)),
    m_Scale(std::move(other.m_Scale)),
    m_Matrix(std::move(other.m_Matrix))
{
    UpdateMatrix();
}

void Transform::SetPosition(glm::vec3& position)
{
    m_Position = position;
    UpdateMatrix();
}

void Transform::SetRotation(glm::vec3& rotation)
{
    m_Rotation = rotation;
    UpdateMatrix();
}

void Transform::SetScale(glm::vec3& scale)
{
    m_Scale = scale;
    UpdateMatrix();
}

void Transform::RotateX(float degree)
{
    m_Rotation.x += degree;
    m_Rotation.x = fmod(m_Rotation.x, 360.0f);
    if (m_Rotation.x < 0) m_Rotation.x += 360.0f;
    UpdateMatrix();
}

void Transform::RotateY(float degree)
{
    m_Rotation.y += degree;
    m_Rotation.y = fmod(m_Rotation.y, 360.0f);
    if (m_Rotation.y < 0) m_Rotation.y += 360.0f;
    UpdateMatrix();
}

void Transform::RotateZ(float degree)
{
    m_Rotation.z += degree;
    m_Rotation.z = fmod(m_Rotation.z, 360.0f);
    if (m_Rotation.z < 0) m_Rotation.z += 360.0f;
    UpdateMatrix();
}

void Transform::TransformX(float amount)
{
    m_Position.x += amount;
    UpdateMatrix();
}

void Transform::TransformY(float amount)
{
    m_Position.y += amount;
    UpdateMatrix();
}

void Transform::TransformZ(float amount)
{
    m_Position.z += amount;
    UpdateMatrix();
}

void Transform::ScaleX(float amount)
{
    m_Scale.x *= amount;
    UpdateMatrix();
}

void Transform::ScaleY(float amount)
{
    m_Scale.y *= amount;
    UpdateMatrix();
}

void Transform::ScaleZ(float amount)
{
    m_Scale.z *= amount;
    UpdateMatrix();
}

void Transform::UpdateMatrix()
{
    m_Matrix = glm::mat4(1.0f);

    m_Matrix = glm::translate(m_Matrix, m_Position);

    m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
    m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
    m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

    m_Matrix = glm::scale(m_Matrix, m_Scale);
}