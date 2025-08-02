#include "Skybox.h"

#include <glm/gtc/matrix_transform.hpp>

#include "BufferLayout.h"
#include "TextureSlot.h"

namespace Lumina
{
    bool Skybox::s_GeometryInitialized = false;
    Ref<VertexArray> Skybox::s_SharedVAO = nullptr;
    Ref<VertexBuffer> Skybox::s_SharedVBO = nullptr;

    static float s_SkyboxVertices[] = 
    {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    void Skybox::BindAttributes(Ref<ShaderProgram> shader, const SkyboxAttributes& attributes)
    {
        LUMINA_ASSERT(shader, "Skybox: Shader cannot be null");

        shader->SetUniformFloat("u_Intensity", attributes.Intensity);
        shader->SetUniformVec3("u_Tint", attributes.Tint);
        shader->SetUniformInt("u_Skybox", TextureSlots::SKYBOX);

        shader->SetUniformFloat("u_Exposure", attributes.Exposure);
        shader->SetUniformFloat("u_Saturation", attributes.Saturation);
        shader->SetUniformFloat("u_Contrast", attributes.Contrast);

        if (attributes.Rotation != 0.0f)
        {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), attributes.Rotation, attributes.RotationAxis);
            shader->SetUniformMat4("u_RotationMatrix", rotationMatrix);
            shader->SetUniformInt("u_HasRotation", 1);
        }
        else
        {
            shader->SetUniformInt("u_HasRotation", 0);
        }

        shader->SetUniformFloat("u_Alpha", attributes.Alpha);

        if (attributes.UseTimeOfDay)
        {
            shader->SetUniformFloat("u_TimeOfDay", attributes.TimeOfDay);
            shader->SetUniformInt("u_UseTimeOfDay", 1);
        }
        else
        {
            shader->SetUniformInt("u_UseTimeOfDay", 0);
        }
    }

    Ref<Skybox> Skybox::Create(const std::string& name)
    {
		auto skybox = CreateRef<Skybox>();
        skybox->SetName(name);

        LUMINA_LOG_INFO("Skybox: Successfully created '{}'", skybox->GetName());
        LUMINA_LOG_INFO("- Creation Type: {}", "Default");
        LUMINA_LOG_INFO("- Valid: {}", skybox->IsValid() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Has Texture: {}", skybox->GetTexture() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Has Geometry: {}", skybox->GetVAO() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Shared Geometry: {}", s_GeometryInitialized ? "Yes" : "No");
        LUMINA_LOG_INFO("- Texture Type: Cubemap");
        LUMINA_LOG_INFO("- Texture Slot: {}", TextureSlots::SKYBOX);

        return skybox;
    }

    Ref<Skybox> Skybox::Create(const std::vector<std::string>& faces, const std::string& name)
    {
		LUMINA_ASSERT(faces.size() == 6, "Skybox: Creation requires exactly 6 face textures");

		auto skybox = CreateRef<Skybox>(faces);
        skybox->SetName(name);

        LUMINA_LOG_INFO("Skybox: Successfully created '{}'", skybox->GetName());
        LUMINA_LOG_INFO("- Creation Type: {}", "Faces Array");
        LUMINA_LOG_INFO("- Valid: {}", skybox->IsValid() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Has Texture: {}", skybox->GetTexture() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Has Geometry: {}", skybox->GetVAO() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Shared Geometry: {}", s_GeometryInitialized ? "Yes" : "No");
        LUMINA_LOG_INFO("- Face Count: {}", faces.size());
		LUMINA_LOG_INFO("- Face Textures: ");
        LUMINA_LOG_INFO("- - Face 0: {}", faces.at(0));
        LUMINA_LOG_INFO("- - Face 1: {}", faces.at(1));
        LUMINA_LOG_INFO("- - Face 2: {}", faces.at(2));
        LUMINA_LOG_INFO("- - Face 3: {}", faces.at(3));
        LUMINA_LOG_INFO("- - Face 4: {}", faces.at(4));
        LUMINA_LOG_INFO("- - Face 5: {}", faces.at(5));
        LUMINA_LOG_INFO("- Texture Type: Cubemap");
        LUMINA_LOG_INFO("- Texture Slot: {}", TextureSlots::SKYBOX);

        return CreateRef<Skybox>(faces);
    }

    Ref<Skybox> Skybox::Create(const Ref<Texture>& cubemapTexture, const std::string& name)
    {
		LUMINA_ASSERT(cubemapTexture, "Skybox: Cannot create skybox with null texture");

        auto skybox = CreateRef<Skybox>(cubemapTexture);
		skybox->SetName(name);

        LUMINA_LOG_INFO("Skybox: Successfully created '{}'", skybox->GetName());
        LUMINA_LOG_INFO("- Creation Type: {}", "Faces Array");
        LUMINA_LOG_INFO("- Valid: {}", skybox->IsValid() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Has Texture: {}", skybox->GetTexture() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Has Geometry: {}", skybox->GetVAO() ? "Yes" : "No");
        LUMINA_LOG_INFO("- Shared Geometry: {}", s_GeometryInitialized ? "Yes" : "No");
        LUMINA_LOG_INFO("- Texture Type: Cubemap");
        LUMINA_LOG_INFO("- Texture Slot: {}", TextureSlots::SKYBOX);

		return skybox;
    }

    Skybox::Skybox()
    {
        CreateGeometry();
        CreateDefaultTexture();
    }

    Skybox::Skybox(const std::vector<std::string>& faces)
    {
        CreateGeometry();
        SetTexture(faces);
    }

    Skybox::Skybox(const Ref<Texture>& cubemapTexture)
    {
        CreateGeometry();
        SetTexture(cubemapTexture);
    }

    void Skybox::SetTexture(const Ref<Texture>& cubemapTexture)
    {
        if (!cubemapTexture)
        {
            LUMINA_LOG_WARN("Cannot set null texture to skybox");
            CreateDefaultTexture();
            return;
        }

        if (!cubemapTexture->IsCubemap())
        {
            LUMINA_LOG_WARN("Texture is not a cubemap, cannot use for skybox");
            CreateDefaultTexture();
            return;
        }

        m_CubemapTexture = cubemapTexture;
    }

    void Skybox::SetTexture(const std::vector<std::string>& faces)
    {
        if (faces.size() != 6)
        {
            LUMINA_LOG_ERROR("Skybox requires exactly 6 face textures, got {0}", faces.size());
            CreateDefaultTexture();
            return;
        }

        auto cubemapTexture = Texture::CreateCubemap(faces);
        if (!cubemapTexture)
        {
            LUMINA_LOG_ERROR("Failed to create cubemap texture from faces");
            CreateDefaultTexture();
            return;
        }

        m_CubemapTexture = cubemapTexture;
    }

    void Skybox::CreateGeometry()
    {
        if (!s_GeometryInitialized)
        {
            InitializeSharedGeometry();
        }

        m_VAO = s_SharedVAO;
        m_VBO = s_SharedVBO;
    }

    void Skybox::CreateDefaultTexture()
    {
        std::vector<uint32_t> whiteData(6, 0xFFFFFFFF);
        m_CubemapTexture = Texture::CreateCubemap(1, 1, whiteData.data());

        if (!m_CubemapTexture)
        {
            LUMINA_LOG_ERROR("Failed to create default skybox texture");
        }
        else
        {
            LUMINA_LOG_INFO("Created default white skybox texture");
        }
    }

    void Skybox::InitializeSharedGeometry()
    {
        if (s_GeometryInitialized)
            return;

        LUMINA_LOG_INFO("Initializing shared skybox geometry...");

        s_SharedVAO = VertexArray::Create();
        s_SharedVBO = VertexBuffer::Create(s_SkyboxVertices, sizeof(s_SkyboxVertices));

        BufferLayout layout = 
        {
            { BufferDataType::Float3, "a_Position" }
        };

        s_SharedVBO->SetLayout(layout);
        s_SharedVAO->SetVertexBuffer(s_SharedVBO);

        s_GeometryInitialized = true;
    }
}