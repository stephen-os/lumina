#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Model.h"
#include "Buffer.h"

#include "Core/Base.h"

namespace Lumina
{
    // (total: 96 bytes, aligned)
    struct InstanceData
    {
        glm::mat4 ModelMatrix = glm::mat4(1.0f);                    // 64 bytes
        glm::vec4 TintColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);    // 16 bytes
        float PointSize = 1.0f;                                     // 4 bytes
        glm::vec3 Padding = { 0.0f, 0.0f, 0.0f };                   // 12 bytes
    };

	class Instance
    {
    public:
        static Ref<Instance> Create(const Ref<Model>& model, uint32_t maxInstances = 1000); 

        Instance(const Ref<Model>& model, uint32_t maxInstances);
        ~Instance() = default;

        void AddInstance(const InstanceData& instance);
        void AddInstance(const ModelAttributes& attributes);
        void Clear();

        bool IsFull() const { return m_InstanceCount >= m_MaxInstances; }
        bool IsEmpty() const { return m_InstanceCount == 0; }
        uint32_t GetInstanceCount() const { return m_InstanceCount; }
        uint32_t GetMaxInstances() const { return m_MaxInstances; }

        void Upload();
        void Render();

        const Ref<Model>& GetModel() const { return m_Model; }

    private: 
        void SetupInstanceBuffer();
        void UpdateVertexArrays();

        InstanceData CreateInstanceData(const ModelAttributes& attributes);

    private:
        Ref<Model> m_Model;
        std::vector<InstanceData> m_InstanceData;
        Ref<VertexBuffer> m_InstanceBuffer;

        uint32_t m_InstanceCount;
        uint32_t m_MaxInstances;
        bool m_DataDirty;
        bool m_IsSetup;
    };
}