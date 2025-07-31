#pragma once

#include <glm/glm.hpp>

#include <sstream>
#include <iomanip>
#include <string>

class Transform
{
public:
    Transform();
    Transform(const Transform& other);
    Transform& operator=(const Transform& other);
    Transform(Transform&& other) noexcept;

    ~Transform() = default;

    void SetPosition(glm::vec3& position);
    void SetRotation(glm::vec3& rotation);
    void SetScale(glm::vec3& scale);

    void RotateX(float degree);
    void RotateY(float degree);
    void RotateZ(float degree);

    void TransformX(float amount);
    void TransformY(float amount);
    void TransformZ(float amount);

    void ScaleX(float amount);
    void ScaleY(float amount);
    void ScaleZ(float amount);

    const glm::mat4& GetMatrix() const { return m_Matrix; };

    const glm::vec3& GetPosition() const { return m_Position; };
    const glm::vec3& GetRotation() const { return m_Rotation; };
    const glm::vec3& GetScale() const { return m_Scale; };

    std::string ToString()
    {
        glm::mat4 matrix = GetMatrix();
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);

        ss << "|-----------------------------|\n";

        for (int i = 0; i < 4; ++i) {
            ss << "| ";
            for (int j = 0; j < 4; ++j) {
                float value = matrix[j][i];
                if (value < 0) {
                    ss << "-" << std::abs(value) << " ";
                }
                else {
                    ss << " " << value << " ";
                }
            }
            ss << "|\n";
        }

        ss << "|-----------------------------|\n";

        return ss.str();
    }
private:
    void UpdateMatrix();

private:
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    glm::vec3 m_Scale;

    glm::mat4 m_Matrix;
};