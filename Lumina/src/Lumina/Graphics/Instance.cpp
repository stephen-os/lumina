#include "Instance.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../Core/Log.h"
#include "../Core/Assert.h"

#include "BufferLayout.h"
#include "RenderCommands.h"

namespace Lumina
{
    Ref<Instance> Instance::Create(const Ref<Model>& model, uint32_t maxInstances)
    {
        return Ref<Instance>::Create(model, maxInstances);
    }

    Instance::Instance(const Ref<Model>& model, uint32_t maxInstances)
        : m_Model(model), m_MaxInstances(maxInstances), m_InstanceCount(0), m_DataDirty(true), m_IsSetup(false)
    {
        LUMINA_ASSERT(model, "Model cannot be null for instance!");
        LUMINA_ASSERT(maxInstances > 0, "Max instance must be greater than 0"); 

        m_InstanceData.reserve(maxInstances);
        SetupInstanceBuffer();
    }

    void Instance::AddInstance(const InstanceData& instance)
    {
        if (m_InstanceCount >= m_MaxInstances)
        {
            LUMINA_LOG_WARN("Instance is full! Cannot add more instances (max: {})", m_MaxInstances);
            return;
        }

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

        size_t dataSize = m_InstanceCount * sizeof(InstanceData);
        m_InstanceBuffer->SetData(m_InstanceData.data(), dataSize);
        m_DataDirty = false;
    }

    void Instance::Render()
    {
        if (m_InstanceCount == 0 || !m_Model)
        {
            LUMINA_LOG_WARN("Cannot render: no instances or null model");
            return;
        }

        Upload();

        if (!m_IsSetup)
        {
            UpdateVertexArrays();
            m_IsSetup = true;
        }

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

            vao->Bind();
			
            bool hasIndices = mesh->HasIndices();
            uint32_t vertexCount = mesh->GetVertexCount();

            if (hasIndices)
            {
                RenderCommands::DrawElementsInstanced(vao, PrimitiveType::Triangles, m_InstanceCount); 
            }
            else
            {
                RenderCommands::DrawArraysInstanced(vao, PrimitiveType::Triangles, vertexCount, m_InstanceCount);
            }

			vao->Unbind();
        }
    }

    void Instance::SetupInstanceBuffer()
    {
        size_t bufferSize = m_MaxInstances * sizeof(InstanceData);
        m_InstanceBuffer = VertexBuffer::Create(bufferSize, BufferUsage::Dynamic);
    }

    void Instance::UpdateVertexArrays()
    {
        // (total: 96 bytes, aligned)
        BufferLayout instanceLayout = 
        {
            { BufferDataType::Mat4,     "a_InstanceMatrix" },       // Model matrix (64 bytes)
            { BufferDataType::Float4,   "a_InstanceColor" },        // Tint color   (16 bytes)
            { BufferDataType::Float,    "a_InstancePointSize" },    // Point size   (4  bytes)
            { BufferDataType::Padding3, "" },                       // Padding      (12 bytes)
        };

        m_InstanceBuffer->SetLayout(instanceLayout);

        const auto& meshes = m_Model->GetMeshes();
        for (const auto& mesh : meshes)
        {
            auto vao = mesh->GetVAO();
            if (!vao)
            {
                LUMINA_LOG_WARN("Mesh has no VAO - cannot setup instance attributes");
                continue;
            }

            vao->SetInstanceBuffer(m_InstanceBuffer);
        }
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