#include "Instance.h"

#include "../Core/Log.h"
#include "../Core/Assert.h"

#include "BufferLayout.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace Lumina
{
    Instance::Instance(const Ref<Model>& model, uint32_t maxInstances)
        : m_Model(model), m_MaxInstances(maxInstances), m_InstanceCount(0), m_DataDirty(true), m_IsSetup(false)
    {
        LUMINA_ASSERT(model, "Model cannot be null for instance!");

        m_InstanceData.reserve(maxInstances);
        SetupInstanceBuffer();

        LUMINA_LOG_INFO("Created Instance for model '{}' with max {} instances",
            model->GetName(), maxInstances);
    }

    void Instance::AddInstance(const InstanceData& instance)
    {
        if (m_InstanceCount >= m_MaxInstances)
        {
            LUMINA_LOG_WARN("Instance is full! Cannot add more instances (max: {})", m_MaxInstances);
            return;
        }

        // Expand vector if needed
        if (m_InstanceCount >= m_InstanceData.size())
        {
            m_InstanceData.push_back(instance);
        }
        else
        {
            m_InstanceData[m_InstanceCount] = instance;
        }

        m_InstanceCount++;
        m_DataDirty = true;
    }

    void Instance::AddInstance(const ModelAttributes& attributes)
    {
        InstanceData instanceData = CreateInstanceData(attributes);
        AddInstance(instanceData);
    }

    void Instance::Clear()
    {
        m_InstanceCount = 0;
        m_DataDirty = true;
    }

    void Instance::Upload()
    {
        if (!m_DataDirty || m_InstanceCount == 0)
            return;

#if 0 
        LUMINA_LOG_INFO("Uploading {} instances to GPU", m_InstanceCount);
        for (uint32_t i = 0; i < std::min(m_InstanceCount, 3u); ++i)
        {
            auto& data = m_InstanceData[i];
            LUMINA_LOG_INFO("Instance {}: Pos=({},{},{}), Color=({},{},{},{})",
                i, data.ModelMatrix[3][0], data.ModelMatrix[3][1], data.ModelMatrix[3][2],
                data.TintColor.r, data.TintColor.g, data.TintColor.b, data.TintColor.a);
        }

        // In Instance::Upload(), add this detailed debug:
        LUMINA_LOG_INFO("=== RAW INSTANCE BUFFER DATA ===");
        for (uint32_t i = 0; i < m_InstanceCount; ++i)
        {
            auto& data = m_InstanceData[i];
            LUMINA_LOG_INFO("Instance {} Matrix:", i);
            LUMINA_LOG_INFO("  [{:8.3f}, {:8.3f}, {:8.3f}, {:8.3f}]",
                data.ModelMatrix[0][0], data.ModelMatrix[0][1], data.ModelMatrix[0][2], data.ModelMatrix[0][3]);
            LUMINA_LOG_INFO("  [{:8.3f}, {:8.3f}, {:8.3f}, {:8.3f}]",
                data.ModelMatrix[1][0], data.ModelMatrix[1][1], data.ModelMatrix[1][2], data.ModelMatrix[1][3]);
            LUMINA_LOG_INFO("  [{:8.3f}, {:8.3f}, {:8.3f}, {:8.3f}]",
                data.ModelMatrix[2][0], data.ModelMatrix[2][1], data.ModelMatrix[2][2], data.ModelMatrix[2][3]);
            LUMINA_LOG_INFO("  [{:8.3f}, {:8.3f}, {:8.3f}, {:8.3f}]",
                data.ModelMatrix[3][0], data.ModelMatrix[3][1], data.ModelMatrix[3][2], data.ModelMatrix[3][3]);
            LUMINA_LOG_INFO("  Color: ({:.3f}, {:.3f}, {:.3f}, {:.3f})",
                data.TintColor.r, data.TintColor.g, data.TintColor.b, data.TintColor.a);
        }

        // Add this debug to see the actual buffer layout being used:
        LUMINA_LOG_INFO("=== BUFFER LAYOUT DEBUG ===");
        LUMINA_LOG_INFO("InstanceData struct size: {}", sizeof(InstanceData));

        const auto& layout = m_InstanceBuffer->GetLayout();
        LUMINA_LOG_INFO("Buffer layout stride: {}", layout.GetStride());

        for (const auto& element : layout.GetElements())
        {
            LUMINA_LOG_INFO("Element '{}': Type={}, Offset={}, Size={}",
                element.Name, (int)element.Type, element.Offset, element.Size);
        }
#endif 

        // Upload instance data to GPU
        size_t dataSize = m_InstanceCount * sizeof(InstanceData);
        m_InstanceBuffer->SetData(m_InstanceData.data(), dataSize);
        m_DataDirty = false;

        LUMINA_LOG_INFO("Uploaded {} instances to GPU ({} bytes)", m_InstanceCount, dataSize);
    }

    void Instance::Render()
    {
        if (m_InstanceCount == 0 || !m_Model)
        {
            LUMINA_LOG_WARN("Cannot render: no instances or null model");
            return;
        }

        // Upload data if needed
        Upload();

        // Setup VAOs for instancing if not done yet
        if (!m_IsSetup)
        {
            UpdateVertexArrays();
            m_IsSetup = true;
        }

        // Render each mesh with instancing
        const auto& meshes = m_Model->GetMeshes();
        for (const auto& mesh : meshes)
        {
            if (!mesh || !mesh->IsSetup())
            {
                LUMINA_LOG_WARN("Skipping invalid or uninitialized mesh");
                continue;
            }

            auto vao = mesh->GetVAO();
            if (!vao)
            {
                LUMINA_LOG_WARN("Mesh has no VAO - cannot render instances");
                continue;
            }

            // Bind VAO and render instances
            vao->Bind();
			
            bool hasIndices = mesh->HasIndices();
            uint32_t vertexCount = mesh->GetVertexCount();

            if (hasIndices)
            {
                // Use indexed drawing
                glDrawElementsInstanced(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr, m_InstanceCount);
            }
            else
            {
                // Use array drawing
                glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, m_InstanceCount);
            }

			vao->Unbind();
        }

        LUMINA_LOG_INFO("Rendered {} instances of model '{}'", m_InstanceCount, m_Model->GetName());
    }

    void Instance::SetupInstanceBuffer()
    {
        // Create instance buffer with dynamic usage for frequent updates
        size_t bufferSize = m_MaxInstances * sizeof(InstanceData);
        m_InstanceBuffer = VertexBuffer::Create(bufferSize, BufferUsage::Dynamic);

        LUMINA_LOG_INFO("Created instance buffer of size {} bytes", bufferSize);
    }

    void Instance::UpdateVertexArrays()
    {
        LUMINA_LOG_INFO("Setting up instance attributes for model '{}'", m_Model->GetName());

        // Create layout for instance data
        BufferLayout instanceLayout = 
        {
            { BufferDataType::Mat4,   "a_InstanceMatrix" },     // Model matrix
            { BufferDataType::Float4, "a_InstanceColor" },      // Tint color
            { BufferDataType::Float,  "a_InstancePointSize" },  // Point size
            { BufferDataType::Float3,  "a_InstancePadding" },   // Padding (12 bytes)
        };

        // Set the layout on our instance buffer
        m_InstanceBuffer->SetLayout(instanceLayout);

        // Add instance buffer to all mesh VAOs
        const auto& meshes = m_Model->GetMeshes();
        for (const auto& mesh : meshes)
        {
            auto vao = mesh->GetVAO();
            if (!vao)
            {
                LUMINA_LOG_WARN("Mesh has no VAO - cannot setup instance attributes");
                continue;
            }

            // Set the instance buffer on the VAO
            // This will automatically set up all instance attributes with divisor = 1
            vao->SetInstanceBuffer(m_InstanceBuffer);
        }

        LUMINA_LOG_INFO("Instance attributes setup complete for {} meshes", meshes.size());
    }

    InstanceData Instance::CreateInstanceData(const ModelAttributes& attributes)
    {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), attributes.Position);

        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), attributes.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), attributes.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), attributes.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rotation = rotationZ * rotationY * rotationX;

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), attributes.Scale);

        glm::mat4 modelMatrix = translation * rotation * scale;

        InstanceData data; 
		data.ModelMatrix = modelMatrix;
		data.TintColor = attributes.TintColor;
		data.PointSize = attributes.PointSize;  

        return data;
    }
}