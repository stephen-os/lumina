#include "Skybox.h"
#include "../Core/Log.h"
#include "BufferLayout.h"

namespace Lumina
{
    // Static member definitions
    bool Skybox::s_GeometryInitialized = false;
    Ref<VertexArray> Skybox::s_SharedVAO = nullptr;
    Ref<VertexBuffer> Skybox::s_SharedVBO = nullptr;

    // Skybox cube vertices (unit cube)
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

    // Factory methods
    Ref<Skybox> Skybox::Create()
    {
        return Ref<Skybox>::Create();
    }

    Ref<Skybox> Skybox::Create(const std::vector<std::string>& faces)
    {
        return Ref<Skybox>::Create(faces);
    }

    Ref<Skybox> Skybox::Create(const Ref<Texture>& cubemapTexture)
    {
        return Ref<Skybox>::Create(cubemapTexture);
    }

    // Constructors
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

    // Texture management
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
        LUMINA_LOG_INFO("Skybox texture set successfully");
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
        LUMINA_LOG_INFO("Skybox created from {0} face textures", faces.size());
    }

    // Private methods
    void Skybox::CreateGeometry()
    {
        // Initialize shared geometry if not already done
        if (!s_GeometryInitialized)
        {
            InitializeSharedGeometry();
        }

        // Use shared geometry for all skybox instances
        m_VAO = s_SharedVAO;
        m_VBO = s_SharedVBO;
    }

    void Skybox::CreateDefaultTexture()
    {
        // Create a simple gradient skybox texture as default
        // This creates a white cubemap that can be tinted
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

        // Create shared VAO and VBO
        s_SharedVAO = VertexArray::Create();
        s_SharedVBO = VertexBuffer::Create(s_SkyboxVertices, sizeof(s_SkyboxVertices));

        // Set up vertex attributes (position only)
        BufferLayout layout = {
            { BufferDataType::Float3, "a_Position" }
        };
        s_SharedVBO->SetLayout(layout);
        s_SharedVAO->SetVertexBuffer(s_SharedVBO);

        s_GeometryInitialized = true;
        LUMINA_LOG_INFO("Shared skybox geometry initialized successfully");
    }
}