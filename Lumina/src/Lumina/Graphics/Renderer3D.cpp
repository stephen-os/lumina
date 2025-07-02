#include "Renderer3D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Core/Log.h"
#include "../Core/Assert.h"
#include "../Utils/FileReader.h"

#include <iostream>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>

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
		Ref<ShaderProgram> PBRIShader = nullptr;
        Ref<ShaderProgram> SkyboxShader = nullptr;

        // Default textures
        Ref<Texture> WhiteTexture = nullptr;
        Ref<Texture> DefaultNormalMap = nullptr;

        // Lighting
        DirectionalLight DirLight;
        std::vector<PointLight> PointLights;

        // Environment
        Ref<Texture> EnvironmentMap = nullptr;
        // Note: No skybox data here anymore - it's in the Skybox class

        // Instance map 
        std::unordered_map<uint64_t, Ref<Instance>> m_ModelInstances;

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

        // Load PBRI shader (for instance rendering)
        {
            std::string vertexSource = ReadFile("res/shaders/PBRI.vert");
            std::string fragmentSource = ReadFile("res/shaders/PBRI.frag");
            s_Data.PBRIShader = ShaderProgram::Create(vertexSource, fragmentSource);
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
        Flush(); 

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

		// Set point size if rendering points
        if (s_Data.CurrentRenderMode == RenderMode::Points)
        {
            float pointSize = (attributes.PointSize > 0.0f) ? attributes.PointSize : s_Data.GlobalPointSize;
            RenderCommands::SetPointSize(pointSize);
            s_Data.PBRShader->SetUniformFloat("u_PointSize", pointSize);
        }

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

    void Renderer3D::DrawSkybox(const Ref<Skybox>& skybox)
    {
         

        if (!skybox || !skybox->IsValid() || !s_Data.SkyboxShader)
        {
            return; // No skybox, invalid skybox, or no shader available
        }

        // Setup render state for skybox
        RenderCommands::SetDepthMask(false);  // Disable depth writing
        RenderCommands::DisableFaceCulling(); // Disable face culling
        RenderCommands::SetDepthFunc(DepthFunc::LessEqual);

        // Bind skybox shader
        s_Data.SkyboxShader->Bind();
        
        // Remove translation from view matrix (keep only rotation)
        glm::mat4 skyboxView = glm::mat4(glm::mat3(s_Data.ViewMatrix));
        glm::mat4 skyboxViewProjection = s_Data.ProjectionMatrix * skyboxView;

        // Set shader uniforms
        s_Data.SkyboxShader->SetUniformMat4("u_ViewProjection", skyboxViewProjection);
        s_Data.SkyboxShader->SetUniformFloat("u_Intensity", skybox->GetIntensity());
        s_Data.SkyboxShader->SetUniformVec3("u_Tint", skybox->GetTint());
        s_Data.SkyboxShader->SetUniformInt("u_Skybox", 6);

        // Bind skybox texture
        auto texture = skybox->GetTexture();
        if (texture)
        {
            texture->Bind(6);
        }

        // Draw skybox geometry
        auto vao = skybox->GetVAO();
        if (vao)
        {
            RenderCommands::DrawTriangles(vao, 36); // 36 vertices for cube
        }

        s_Data.SkyboxShader->Unbind();

        // Restore render state
        RenderCommands::EnableFaceCulling();
        RenderCommands::SetDepthMask(true);
		RenderCommands::SetDepthFunc(DepthFunc::Less);

        // Update statistics
        s_Data.Stats.DrawCalls++;
        s_Data.Stats.TriangleCount += 12; // 6 faces * 2 triangles per face
    }

    void Renderer3D::SubmitModel(const Ref<Model>& model, const ModelAttributes& attributes)
    {
        if (!model || model->IsEmpty())
            return;

        // Get or create instance batch for this model
        auto instance = GetInstance(model);

        // Add this model's attributes to the batch
        instance->AddInstance(attributes);

        // If batch is full, flush it immediately
        if (instance->IsFull())
        {
            // Bind the INSTANCED shader
            s_Data.PBRIShader->Bind();

            // Set common uniforms (no u_Model needed!)
            s_Data.PBRIShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
            s_Data.PBRIShader->SetUniformVec3("u_CameraPos", s_Data.CameraPosition);
            s_Data.PBRIShader->SetUniformInt("u_RenderMode", static_cast<int>(s_Data.CurrentRenderMode));

            // Set fallback tint color (instances will override this)
            s_Data.PBRIShader->SetUniformVec4("u_TintColor", glm::vec4(1.0f));

            // Set lighting uniforms
            s_Data.PBRIShader->SetUniformVec3("u_DirLight.Direction", s_Data.DirLight.Direction);
            s_Data.PBRIShader->SetUniformVec3("u_DirLight.Color", s_Data.DirLight.Color);
            s_Data.PBRIShader->SetUniformFloat("u_DirLight.Intensity", s_Data.DirLight.Intensity);

            // Set point lights
            s_Data.PBRIShader->SetUniformInt("u_NumPointLights", static_cast<int>(s_Data.PointLights.size()));
            for (size_t i = 0; i < s_Data.PointLights.size() && i < MaxPointLights; ++i)
            {
                std::string base = "u_PointLights[" + std::to_string(i) + "]";
                s_Data.PBRIShader->SetUniformVec3(base + ".Position", s_Data.PointLights[i].Position);
                s_Data.PBRIShader->SetUniformVec3(base + ".Color", s_Data.PointLights[i].Color);
                s_Data.PBRIShader->SetUniformFloat(base + ".Intensity", s_Data.PointLights[i].Intensity);
                s_Data.PBRIShader->SetUniformFloat(base + ".Constant", s_Data.PointLights[i].Constant);
                s_Data.PBRIShader->SetUniformFloat(base + ".Linear", s_Data.PointLights[i].Linear);
                s_Data.PBRIShader->SetUniformFloat(base + ".Quadratic", s_Data.PointLights[i].Quadratic);
            }

            // Set environment mapping
            if (s_Data.EnvironmentMap)
            {
                s_Data.EnvironmentMap->Bind(5);
                s_Data.PBRIShader->SetUniformInt("u_EnvironmentMap", 5);
                s_Data.PBRIShader->SetUniformInt("u_HasEnvironmentMap", 1);
            }
            else
            {
                s_Data.PBRIShader->SetUniformInt("u_HasEnvironmentMap", 0);
            }

            // Set point size for points mode
            if (s_Data.CurrentRenderMode == RenderMode::Points)
            {
                s_Data.PBRIShader->SetUniformFloat("u_PointSize", s_Data.GlobalPointSize);
            }

            // Render all instance batches
            
            if (!instance->IsEmpty())
            {
                // For each model, we need to set up its material textures
                auto model = instance->GetModel();
                if (model && !model->GetMeshes().empty())
                {
                    // Use the first mesh's material for all instances of this model
                    auto mesh = model->GetMeshes()[0];
                    auto material = mesh->GetMaterial();

                    // Bind material textures (same as in DrawMesh)
                    if (material && material->GetAlbedoTexture())
                    {
                        material->GetAlbedoTexture()->Bind(0);
                        s_Data.PBRIShader->SetUniformInt("u_AlbedoTexture", 0);
                        s_Data.PBRIShader->SetUniformInt("u_HasAlbedoTexture", 1);
                    }
                    else
                    {
                        s_Data.WhiteTexture->Bind(0);
                        s_Data.PBRIShader->SetUniformInt("u_AlbedoTexture", 0);
                        s_Data.PBRIShader->SetUniformInt("u_HasAlbedoTexture", 0);
                    }

                    if (material && material->GetNormalTexture())
                    {
                        material->GetNormalTexture()->Bind(1);
                        s_Data.PBRIShader->SetUniformInt("u_NormalTexture", 1);
                        s_Data.PBRIShader->SetUniformInt("u_HasNormalTexture", 1);
                    }
                    else
                    {
                        s_Data.DefaultNormalMap->Bind(1);
                        s_Data.PBRIShader->SetUniformInt("u_NormalTexture", 1);
                        s_Data.PBRIShader->SetUniformInt("u_HasNormalTexture", 0);
                    }

                    if (material && material->GetMetallicTexture())
                    {
                        material->GetMetallicTexture()->Bind(2);
                        s_Data.PBRIShader->SetUniformInt("u_MetallicTexture", 2);
                        s_Data.PBRIShader->SetUniformInt("u_HasMetallicTexture", 1);
                    }
                    else
                    {
                        s_Data.WhiteTexture->Bind(2);
                        s_Data.PBRIShader->SetUniformInt("u_MetallicTexture", 2);
                        s_Data.PBRIShader->SetUniformInt("u_HasMetallicTexture", 0);
                    }

                    if (material && material->GetRoughnessTexture())
                    {
                        material->GetRoughnessTexture()->Bind(3);
                        s_Data.PBRIShader->SetUniformInt("u_RoughnessTexture", 3);
                        s_Data.PBRIShader->SetUniformInt("u_HasRoughnessTexture", 1);
                    }
                    else
                    {
                        s_Data.WhiteTexture->Bind(3);
                        s_Data.PBRIShader->SetUniformInt("u_RoughnessTexture", 3);
                        s_Data.PBRIShader->SetUniformInt("u_HasRoughnessTexture", 0);
                    }

                    if (material && material->GetAOTexture())
                    {
                        material->GetAOTexture()->Bind(4);
                        s_Data.PBRIShader->SetUniformInt("u_AOTexture", 4);
                        s_Data.PBRIShader->SetUniformInt("u_HasAOTexture", 1);
                    }
                    else
                    {
                        s_Data.WhiteTexture->Bind(4);
                        s_Data.PBRIShader->SetUniformInt("u_AOTexture", 4);
                        s_Data.PBRIShader->SetUniformInt("u_HasAOTexture", 0);
                    }

                    // Set material properties
                    if (material)
                    {
                        s_Data.PBRIShader->SetUniformVec3("u_Material.Albedo", material->GetAlbedo());
                        s_Data.PBRIShader->SetUniformFloat("u_Material.Metallic", material->GetMetallic());
                        s_Data.PBRIShader->SetUniformFloat("u_Material.Roughness", material->GetRoughness());
                        s_Data.PBRIShader->SetUniformFloat("u_Material.AO", material->GetAO());
                    }
                    else
                    {
                        // Default material properties
                        s_Data.PBRIShader->SetUniformVec3("u_Material.Albedo", glm::vec3(1.0f));
                        s_Data.PBRIShader->SetUniformFloat("u_Material.Metallic", 0.0f);
                        s_Data.PBRIShader->SetUniformFloat("u_Material.Roughness", 0.5f);
                        s_Data.PBRIShader->SetUniformFloat("u_Material.AO", 1.0f);
                    }
                }

                instance->Upload();
                instance->Render();
                instance->Clear();

                // Update stats
                s_Data.Stats.DrawCalls++;
                s_Data.Stats.ModelCount += instance->GetInstanceCount();
            }

            s_Data.PBRIShader->Unbind();
        }
    }

    Ref<Instance> Renderer3D::GetInstance(const Ref<Model>& model)
    {
        uint64_t modelUUID = model->GetUUID();
        auto it = s_Data.m_ModelInstances.find(modelUUID);
        if (it != s_Data.m_ModelInstances.end())
            return it->second;

        // Create new instance batch for this model
        auto instance = Ref<Instance>::Create(model);
        s_Data.m_ModelInstances[modelUUID] = instance;
        return instance;
    }

    void Renderer3D::Flush()
    {
        if (s_Data.m_ModelInstances.empty())
            return;

        // Bind the INSTANCED shader
        s_Data.PBRIShader->Bind();

        // Set common uniforms (no u_Model needed!)
        s_Data.PBRIShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
        s_Data.PBRIShader->SetUniformVec3("u_CameraPos", s_Data.CameraPosition);
        s_Data.PBRIShader->SetUniformInt("u_RenderMode", static_cast<int>(s_Data.CurrentRenderMode));

        // Set fallback tint color (instances will override this)
        s_Data.PBRIShader->SetUniformVec4("u_TintColor", glm::vec4(1.0f));

        // Set lighting uniforms
        s_Data.PBRIShader->SetUniformVec3("u_DirLight.Direction", s_Data.DirLight.Direction);
        s_Data.PBRIShader->SetUniformVec3("u_DirLight.Color", s_Data.DirLight.Color);
        s_Data.PBRIShader->SetUniformFloat("u_DirLight.Intensity", s_Data.DirLight.Intensity);

        // Set point lights
        s_Data.PBRIShader->SetUniformInt("u_NumPointLights", static_cast<int>(s_Data.PointLights.size()));
        for (size_t i = 0; i < s_Data.PointLights.size() && i < MaxPointLights; ++i)
        {
            std::string base = "u_PointLights[" + std::to_string(i) + "]";
            s_Data.PBRIShader->SetUniformVec3(base + ".Position", s_Data.PointLights[i].Position);
            s_Data.PBRIShader->SetUniformVec3(base + ".Color", s_Data.PointLights[i].Color);
            s_Data.PBRIShader->SetUniformFloat(base + ".Intensity", s_Data.PointLights[i].Intensity);
            s_Data.PBRIShader->SetUniformFloat(base + ".Constant", s_Data.PointLights[i].Constant);
            s_Data.PBRIShader->SetUniformFloat(base + ".Linear", s_Data.PointLights[i].Linear);
            s_Data.PBRIShader->SetUniformFloat(base + ".Quadratic", s_Data.PointLights[i].Quadratic);
        }

        // Set environment mapping
        if (s_Data.EnvironmentMap)
        {
            s_Data.EnvironmentMap->Bind(5);
            s_Data.PBRIShader->SetUniformInt("u_EnvironmentMap", 5);
            s_Data.PBRIShader->SetUniformInt("u_HasEnvironmentMap", 1);
        }
        else
        {
            s_Data.PBRIShader->SetUniformInt("u_HasEnvironmentMap", 0);
        }

        // Set point size for points mode
        if (s_Data.CurrentRenderMode == RenderMode::Points)
        {
            s_Data.PBRIShader->SetUniformFloat("u_PointSize", s_Data.GlobalPointSize);
        }

        // Render all instance batches
        for (auto& [uuid, instance] : s_Data.m_ModelInstances)
        {
            if (!instance->IsEmpty())
            {
                // For each model, we need to set up its material textures
                auto model = instance->GetModel();
                if (model && !model->GetMeshes().empty())
                {
                    // Use the first mesh's material for all instances of this model
                    auto mesh = model->GetMeshes()[0];
                    auto material = mesh->GetMaterial();

                    // Bind material textures (same as in DrawMesh)
                    if (material && material->GetAlbedoTexture())
                    {
                        material->GetAlbedoTexture()->Bind(0);
                        s_Data.PBRIShader->SetUniformInt("u_AlbedoTexture", 0);
                        s_Data.PBRIShader->SetUniformInt("u_HasAlbedoTexture", 1);
                    }
                    else
                    {
                        s_Data.WhiteTexture->Bind(0);
                        s_Data.PBRIShader->SetUniformInt("u_AlbedoTexture", 0);
                        s_Data.PBRIShader->SetUniformInt("u_HasAlbedoTexture", 0);
                    }

                    if (material && material->GetNormalTexture())
                    {
                        material->GetNormalTexture()->Bind(1);
                        s_Data.PBRIShader->SetUniformInt("u_NormalTexture", 1);
                        s_Data.PBRIShader->SetUniformInt("u_HasNormalTexture", 1);
                    }
                    else
                    {
                        s_Data.DefaultNormalMap->Bind(1);
                        s_Data.PBRIShader->SetUniformInt("u_NormalTexture", 1);
                        s_Data.PBRIShader->SetUniformInt("u_HasNormalTexture", 0);
                    }

                    if (material && material->GetMetallicTexture())
                    {
                        material->GetMetallicTexture()->Bind(2);
                        s_Data.PBRIShader->SetUniformInt("u_MetallicTexture", 2);
                        s_Data.PBRIShader->SetUniformInt("u_HasMetallicTexture", 1);
                    }
                    else
                    {
                        s_Data.WhiteTexture->Bind(2);
                        s_Data.PBRIShader->SetUniformInt("u_MetallicTexture", 2);
                        s_Data.PBRIShader->SetUniformInt("u_HasMetallicTexture", 0);
                    }

                    if (material && material->GetRoughnessTexture())
                    {
                        material->GetRoughnessTexture()->Bind(3);
                        s_Data.PBRIShader->SetUniformInt("u_RoughnessTexture", 3);
                        s_Data.PBRIShader->SetUniformInt("u_HasRoughnessTexture", 1);
                    }
                    else
                    {
                        s_Data.WhiteTexture->Bind(3);
                        s_Data.PBRIShader->SetUniformInt("u_RoughnessTexture", 3);
                        s_Data.PBRIShader->SetUniformInt("u_HasRoughnessTexture", 0);
                    }

                    if (material && material->GetAOTexture())
                    {
                        material->GetAOTexture()->Bind(4);
                        s_Data.PBRIShader->SetUniformInt("u_AOTexture", 4);
                        s_Data.PBRIShader->SetUniformInt("u_HasAOTexture", 1);
                    }
                    else
                    {
                        s_Data.WhiteTexture->Bind(4);
                        s_Data.PBRIShader->SetUniformInt("u_AOTexture", 4);
                        s_Data.PBRIShader->SetUniformInt("u_HasAOTexture", 0);
                    }

                    // Set material properties
                    if (material)
                    {
                        s_Data.PBRIShader->SetUniformVec3("u_Material.Albedo", material->GetAlbedo());
                        s_Data.PBRIShader->SetUniformFloat("u_Material.Metallic", material->GetMetallic());
                        s_Data.PBRIShader->SetUniformFloat("u_Material.Roughness", material->GetRoughness());
                        s_Data.PBRIShader->SetUniformFloat("u_Material.AO", material->GetAO());
                    }
                    else
                    {
                        // Default material properties
                        s_Data.PBRIShader->SetUniformVec3("u_Material.Albedo", glm::vec3(1.0f));
                        s_Data.PBRIShader->SetUniformFloat("u_Material.Metallic", 0.0f);
                        s_Data.PBRIShader->SetUniformFloat("u_Material.Roughness", 0.5f);
                        s_Data.PBRIShader->SetUniformFloat("u_Material.AO", 1.0f);
                    }
                }

                instance->Upload();
                instance->Render();
                instance->Clear();

                // Update stats
                s_Data.Stats.DrawCalls++;
                s_Data.Stats.ModelCount += instance->GetInstanceCount();
            }
        }

        s_Data.PBRIShader->Unbind();
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

    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }

    void Renderer3D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
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