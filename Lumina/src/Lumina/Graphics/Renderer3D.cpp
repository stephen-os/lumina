#include "Renderer3D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Core/Log.h"
#include "../Core/Assert.h"
#include "../Utils/FileReader.h"

#include <iostream>
#include <vector>

namespace Lumina
{
    // Constants
    constexpr uint32_t MaxPointLights = 32;

    // Renderer data storage
    struct Renderer3DData
    {
        // Core framebuffer
        Ref<FrameBuffer> RendererFrameBuffer;

        // Shaders
        Ref<ShaderProgram> PBRShader = nullptr;
        Ref<ShaderProgram> SkyboxShader = nullptr;

        // Default textures
        Ref<Texture> WhiteTexture = nullptr;
        Ref<Texture> DefaultNormalMap = nullptr;

        // Lighting
        DirectionalLight DirLight;
        std::vector<PointLight> PointLights;

        // Environment
        Ref<Texture> EnvironmentMap = nullptr;
        Ref<Texture> SkyboxTexture = nullptr;

        // Render settings
        RenderMode CurrentRenderMode = RenderMode::Normal;
        float GlobalPointSize = 1.0f;

        // View Projection
        glm::mat4 ViewMatrix = glm::mat4(1.0f);
        glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);
        glm::vec3 CameraPosition = glm::vec3(0.0f);

        // Framebuffer Resolution
        uint32_t Width = 800;
        uint32_t Height = 600;

        // Statistics
        Renderer3D::Statistics Stats;
    };

    static Renderer3DData s_Data;

    void Renderer3D::Init()
    {
        LUMINA_LOG_INFO("Initializing 3D Renderer...");

        // Create framebuffer
        s_Data.RendererFrameBuffer = FrameBuffer::Create();

        // Create default white texture
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture = Texture::Create(1, 1);
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        // Create default normal map (pointing up in tangent space)
        uint32_t normalData = 0xFF8080FF; // RGB(128, 128, 255) = normal pointing up
        s_Data.DefaultNormalMap = Texture::Create(1, 1);
        s_Data.DefaultNormalMap->SetData(&normalData, sizeof(uint32_t));

        // Load PBR shader
        {
            std::string vertexSource = ReadFile("res/shaders/PBR.vert");
            std::string fragmentSource = ReadFile("res/shaders/PBR.frag");
            s_Data.PBRShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Load Skybox shader
        {
            std::string vertexSource = ReadFile("res/shaders/Skybox.vert");
            std::string fragmentSource = ReadFile("res/shaders/Skybox.frag");
            s_Data.SkyboxShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Set default lighting
        s_Data.DirLight.Direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
        s_Data.DirLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
        s_Data.DirLight.Intensity = 3.0f;

        LUMINA_LOG_INFO("3D Renderer initialized successfully");
    }

    void Renderer3D::Shutdown()
    {
        LUMINA_LOG_INFO("Shutting down 3D Renderer...");
        s_Data.PointLights.clear();
    }

    void Renderer3D::Begin(Camera& camera)
    {
        s_Data.ViewMatrix = camera.GetViewMatrix();
        s_Data.ProjectionMatrix = camera.GetProjectionMatrix();
        s_Data.ViewProjectionMatrix = s_Data.ProjectionMatrix * s_Data.ViewMatrix;
        s_Data.CameraPosition = camera.GetPosition();

        // Reset stats
        ResetStats();

        // Setup framebuffer
        s_Data.RendererFrameBuffer->Bind();
        s_Data.RendererFrameBuffer->Resize(s_Data.Width, s_Data.Height);

        RenderCommands::SetViewport(0, 0, s_Data.Width, s_Data.Height);
        RenderCommands::SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        RenderCommands::Clear();
        RenderCommands::EnableDepthTest();
        RenderCommands::EnableProgramPointSize();
    }

    void Renderer3D::Begin(const glm::mat4& viewProjection)
    {
        s_Data.ViewProjectionMatrix = viewProjection;

        // Reset stats
        ResetStats();

        // Setup framebuffer
        s_Data.RendererFrameBuffer->Bind();
        s_Data.RendererFrameBuffer->Resize(s_Data.Width, s_Data.Height);

        RenderCommands::SetViewport(0, 0, s_Data.Width, s_Data.Height);
        RenderCommands::SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        RenderCommands::Clear();
        RenderCommands::EnableDepthTest();
        RenderCommands::EnableProgramPointSize();
    }

    void Renderer3D::End()
    {
        s_Data.RendererFrameBuffer->Unbind();
    }

    void Renderer3D::DrawModel(const Ref<Model>& model, const ModelAttributes& attributes)
    {
        if (!model)
        {
            LUMINA_LOG_WARN("Attempted to draw null model");
            return;
        }

        glm::mat4 modelMatrix = CalculateModelMatrix(attributes);

        s_Data.PBRShader->Bind();

        // Set matrices
        s_Data.PBRShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
        s_Data.PBRShader->SetUniformMat4("u_Model", modelMatrix);

        // Set camera position
        s_Data.PBRShader->SetUniformVec3("u_CameraPos", s_Data.CameraPosition);

        // Set global tint color
        s_Data.PBRShader->SetUniformVec4("u_TintColor", attributes.TintColor);

        // Set render mode in shader
        s_Data.PBRShader->SetUniformInt("u_RenderMode", static_cast<int>(s_Data.CurrentRenderMode));

        SetupLighting();
        SetupRenderMode(attributes);

        const auto& meshes = model->GetMeshes();
        for (const auto& mesh : meshes)
        {
            DrawMesh(mesh, attributes);
        }

        s_Data.PBRShader->Unbind();

        s_Data.Stats.ModelCount++;
        s_Data.Stats.DrawCalls++;
    }

    void Renderer3D::DrawMesh(const Ref<Mesh>& mesh, const ModelAttributes& attributes)
    {
        if (!mesh || !mesh->IsSetup())
        {
            LUMINA_LOG_WARN("Attempted to draw null or uninitialized mesh");
            return;
        }

        auto material = mesh->GetMaterial();

        // Bind material textures
        if (material && material->GetAlbedoTexture())
        {
            material->GetAlbedoTexture()->Bind(0);
            s_Data.PBRShader->SetUniformInt("u_AlbedoTexture", 0);
            s_Data.PBRShader->SetUniformInt("u_HasAlbedoTexture", 1);
        }
        else
        {
            s_Data.WhiteTexture->Bind(0);
            s_Data.PBRShader->SetUniformInt("u_AlbedoTexture", 0);
            s_Data.PBRShader->SetUniformInt("u_HasAlbedoTexture", 0);
        }

        if (material && material->GetNormalTexture())
        {
            material->GetNormalTexture()->Bind(1);
            s_Data.PBRShader->SetUniformInt("u_NormalTexture", 1);
            s_Data.PBRShader->SetUniformInt("u_HasNormalTexture", 1);
        }
        else
        {
            s_Data.DefaultNormalMap->Bind(1);
            s_Data.PBRShader->SetUniformInt("u_NormalTexture", 1);
            s_Data.PBRShader->SetUniformInt("u_HasNormalTexture", 0);
        }

        if (material && material->GetMetallicTexture())
        {
            material->GetMetallicTexture()->Bind(2);
            s_Data.PBRShader->SetUniformInt("u_MetallicTexture", 2);
            s_Data.PBRShader->SetUniformInt("u_HasMetallicTexture", 1);
        }
        else
        {
            s_Data.WhiteTexture->Bind(2);
            s_Data.PBRShader->SetUniformInt("u_MetallicTexture", 2);
            s_Data.PBRShader->SetUniformInt("u_HasMetallicTexture", 0);
        }

        if (material && material->GetRoughnessTexture())
        {
            material->GetRoughnessTexture()->Bind(3);
            s_Data.PBRShader->SetUniformInt("u_RoughnessTexture", 3);
            s_Data.PBRShader->SetUniformInt("u_HasRoughnessTexture", 1);
        }
        else
        {
            s_Data.WhiteTexture->Bind(3);
            s_Data.PBRShader->SetUniformInt("u_RoughnessTexture", 3);
            s_Data.PBRShader->SetUniformInt("u_HasRoughnessTexture", 0);
        }

        if (material && material->GetAOTexture())
        {
            material->GetAOTexture()->Bind(4);
            s_Data.PBRShader->SetUniformInt("u_AOTexture", 4);
            s_Data.PBRShader->SetUniformInt("u_HasAOTexture", 1);
        }
        else
        {
            s_Data.WhiteTexture->Bind(4);
            s_Data.PBRShader->SetUniformInt("u_AOTexture", 4);
            s_Data.PBRShader->SetUniformInt("u_HasAOTexture", 0);
        }

        // Set material properties
        if (material)
        {
            s_Data.PBRShader->SetUniformVec3("u_Material.Albedo", material->GetAlbedo());
            s_Data.PBRShader->SetUniformFloat("u_Material.Metallic", material->GetMetallic());
            s_Data.PBRShader->SetUniformFloat("u_Material.Roughness", material->GetRoughness());
            s_Data.PBRShader->SetUniformFloat("u_Material.AO", material->GetAO());
        }
        else
        {
            // Default material properties
            s_Data.PBRShader->SetUniformVec3("u_Material.Albedo", glm::vec3(1.0f, 1.0f, 1.0f));
            s_Data.PBRShader->SetUniformFloat("u_Material.Metallic", 0.0f);
            s_Data.PBRShader->SetUniformFloat("u_Material.Roughness", 0.5f);
            s_Data.PBRShader->SetUniformFloat("u_Material.AO", 1.0f);
        }

        // Get VAO from mesh
        auto vao = mesh->GetVAO();
        if (!vao)
        {
            LUMINA_LOG_WARN("Mesh has no VAO setup");
            return;
        }

        // Draw based on render mode
        vao->Bind();

        bool hasIndices = mesh->HasIndices();
        size_t indexCount = mesh->GetIndexCount();
        size_t vertexCount = mesh->GetVertexCount();

        switch (s_Data.CurrentRenderMode)
        {
        case RenderMode::Normal:
        {
            RenderCommands::SetPolygonMode(PolygonMode::Fill);
            if (hasIndices)
            {
                RenderCommands::DrawTrianglesIndexed(vao);
                s_Data.Stats.TriangleCount += indexCount / 3;
            }
            else
            {
                RenderCommands::DrawTriangles(vao, vertexCount);
                s_Data.Stats.TriangleCount += vertexCount / 3;
            }
            break;
        }
        case RenderMode::Wireframe:
        {
            RenderCommands::SetPolygonMode(PolygonMode::Line);
            if (hasIndices)
            {
                RenderCommands::DrawTrianglesIndexed(vao);
                s_Data.Stats.TriangleCount += indexCount / 3;
            }
            else
            {
                RenderCommands::DrawTriangles(vao, vertexCount);
                s_Data.Stats.TriangleCount += vertexCount / 3;
            }
            break;
        }
        case RenderMode::Points:
        {
            RenderCommands::SetPolygonMode(PolygonMode::Fill); // Reset polygon mode
            if (hasIndices)
            {
                RenderCommands::DrawPointsIndexed(vao);
            }
            else
            {
                RenderCommands::DrawPoints(vao, vertexCount);
            }
            break;
        }
        }

        vao->Unbind();

        s_Data.Stats.MeshCount++;
        s_Data.Stats.VertexCount += vertexCount;
        s_Data.Stats.TexturesUsed += 5; // Always bind 5 texture slots
    }

    void Renderer3D::SetResolution(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            LUMINA_LOG_WARN("Invalid resolution: {0}x{1}", width, height);
            return;
        }

        s_Data.Width = width;
        s_Data.Height = height;
    }

    glm::vec2 Renderer3D::GetResolution()
    {
        return { s_Data.Width, s_Data.Height };
    }

    void Renderer3D::SetRenderMode(RenderMode mode)
    {
        s_Data.CurrentRenderMode = mode;
    }

    RenderMode Renderer3D::GetRenderMode()
    {
        return s_Data.CurrentRenderMode;
    }

    void Renderer3D::SetGlobalPointSize(float size)
    {
        s_Data.GlobalPointSize = size;
    }

    float Renderer3D::GetGlobalPointSize()
    {
        return s_Data.GlobalPointSize;
    }

    uint32_t Renderer3D::GetImage()
    {
        return s_Data.RendererFrameBuffer->GetColorAttachment();
    }

    void Renderer3D::SetDirectionalLight(const DirectionalLight& light)
    {
        s_Data.DirLight = light;
    }

    void Renderer3D::AddPointLight(const PointLight& light)
    {
        if (s_Data.PointLights.size() < MaxPointLights)
        {
            s_Data.PointLights.push_back(light);
        }
        else
        {
            LUMINA_LOG_WARN("Maximum number of point lights ({0}) reached!", MaxPointLights);
        }
    }

    void Renderer3D::ClearPointLights()
    {
        s_Data.PointLights.clear();
    }

    void Renderer3D::SetEnvironmentMap(const Ref<Texture>& envMap)
    {
        s_Data.EnvironmentMap = envMap;
    }

    void Renderer3D::SetSkybox(const Ref<Texture>& skybox)
    {
        s_Data.SkyboxTexture = skybox;
    }

    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }

    void Renderer3D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    void Renderer3D::SetupLighting()
    {
        // Set directional light
        s_Data.PBRShader->SetUniformVec3("u_DirLight.Direction", s_Data.DirLight.Direction);
        s_Data.PBRShader->SetUniformVec3("u_DirLight.Color", s_Data.DirLight.Color);
        s_Data.PBRShader->SetUniformFloat("u_DirLight.Intensity", s_Data.DirLight.Intensity);

        // Set point lights
        s_Data.PBRShader->SetUniformInt("u_NumPointLights", static_cast<int>(s_Data.PointLights.size()));

        for (size_t i = 0; i < s_Data.PointLights.size() && i < MaxPointLights; ++i)
        {
            std::string base = "u_PointLights[" + std::to_string(i) + "]";
            s_Data.PBRShader->SetUniformVec3(base + ".Position", s_Data.PointLights[i].Position);
            s_Data.PBRShader->SetUniformVec3(base + ".Color", s_Data.PointLights[i].Color);
            s_Data.PBRShader->SetUniformFloat(base + ".Intensity", s_Data.PointLights[i].Intensity);
            s_Data.PBRShader->SetUniformFloat(base + ".Constant", s_Data.PointLights[i].Constant);
            s_Data.PBRShader->SetUniformFloat(base + ".Linear", s_Data.PointLights[i].Linear);
            s_Data.PBRShader->SetUniformFloat(base + ".Quadratic", s_Data.PointLights[i].Quadratic);
        }

        // Set environment mapping if available
        if (s_Data.EnvironmentMap)
        {
            s_Data.EnvironmentMap->Bind(5);
            s_Data.PBRShader->SetUniformInt("u_EnvironmentMap", 5);
            s_Data.PBRShader->SetUniformInt("u_HasEnvironmentMap", 1);
        }
        else
        {
            s_Data.PBRShader->SetUniformInt("u_HasEnvironmentMap", 0);
        }
    }

    void Renderer3D::SetupRenderMode(const ModelAttributes& attributes)
    {
        if (s_Data.CurrentRenderMode == RenderMode::Points)
        {
            // Set point size - use per-object size if specified, otherwise global
            float pointSize = (attributes.PointSize > 0.0f) ? attributes.PointSize : s_Data.GlobalPointSize;
            RenderCommands::SetPointSize(pointSize);
            s_Data.PBRShader->SetUniformFloat("u_PointSize", pointSize);
        }
    }

    glm::mat4 Renderer3D::CalculateModelMatrix(const ModelAttributes& attributes)
    {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), attributes.Position);

        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), attributes.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), attributes.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), attributes.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rotation = rotationZ * rotationY * rotationX;

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), attributes.Scale);

        return translation * rotation * scale;
    }
}