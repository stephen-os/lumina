#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Model.h"
#include "Buffer.h"
#include "../Core/Ref.h"

namespace Lumina
{
    struct InstanceData
    {
        glm::mat4 ModelMatrix = glm::mat4(1.0f);                    // 64 bytes
        glm::vec4 TintColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);    // 16 bytes
        float PointSize = 1.0f;                                     // 4 bytes
        float Padding[3] = { 0.0f, 0.0f, 0.0f };                    // 12 bytes (total: 96 bytes, aligned)
    };

	class Instance : public Referencable
    {
    public:
        Instance(const Ref<Model>& model, uint32_t maxInstances = 1000);
        ~Instance() = default;

        // Instance management
        void AddInstance(const InstanceData& instance);
        void AddInstance(const ModelAttributes& attributes);
        void Clear();

        // State queries
        bool IsFull() const { return m_InstanceCount >= m_MaxInstances; }
        bool IsEmpty() const { return m_InstanceCount == 0; }
        uint32_t GetInstanceCount() const { return m_InstanceCount; }
        uint32_t GetMaxInstances() const { return m_MaxInstances; }

        void Upload();  // Upload instance data to GPU
        void Render();  // Render all instances

        const Ref<Model>& GetModel() const { return m_Model; }

    private:
        // Core data
        Ref<Model> m_Model;
        std::vector<InstanceData> m_InstanceData;
        Ref<VertexBuffer> m_InstanceBuffer;

        // State
        uint32_t m_InstanceCount;
        uint32_t m_MaxInstances;
        bool m_DataDirty;
        bool m_IsSetup;

        // Setup methods
        void SetupInstanceBuffer();
        void UpdateVertexArrays();

        // Helper methods
        InstanceData CreateInstanceData(const ModelAttributes& attributes);
    };
}