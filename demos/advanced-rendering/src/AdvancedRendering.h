#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include "imgui.h"
#include "Lumina/Lumina.h"
#include <glm/glm.hpp>

namespace Lumina
{
    class AdvancedRendering : public Layer
    {
    public:
        virtual void OnAttach() override
        {
			m_OrthographicCamera = CreateRef<OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f);

            m_OrthographicCamera->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
            m_OrthographicCamera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_OrthographicCamera->SetZoom(150.0f);

			std::string atlasPath = "res/texture/factory_atlas.png";
			m_TextureAtlas = TextureAtlas::Create(atlasPath, 16, 16);
        }

        virtual void OnDetach() override
        {
            
        }

        virtual void OnUpdate(float ts) override
        {
            Renderer2D::ResetStats();

			m_FPS = 1.0f / m_FrameTimer.Elapsed();
            m_FrameTimer.Reset(); 
        }

        virtual void OnUIRender() override
        {
            // Main rendering viewport
            ImGui::Begin("Scene Viewer");
            ImGui::SetCursorPos({ 0.0f, 0.0f });
            ImVec2 size = ImGui::GetContentRegionAvail();

            m_OrthographicCamera->SetSize(size.x, size.y);

            Renderer2D::SetResolution(size.x, size.y);
            Renderer2D::SetRenderMode(PolygonMode::Fill);
            Renderer2D::Begin(m_OrthographicCamera);

            // Draw all objects based on enabled states
            if (m_QuadEnabled)
            {
				Renderer2D::SetQuadTexture(m_TextureAtlas->GetTexture());
				Renderer2D::SetQuadTextureCoords(m_TextureAtlas->GetTextureCoords(0));
                Renderer2D::SetQuadPosition(m_QuadPosition);
                Renderer2D::SetQuadRotation(m_QuadRotation);
                Renderer2D::SetQuadSize(m_QuadSize);
                Renderer2D::SetQuadTintColor(glm::vec4(m_QuadColor[0], m_QuadColor[1], m_QuadColor[2], m_QuadColor[3]));
                Renderer2D::DrawQuad();
            }

            if (m_CircleEnabled)
            {
                Renderer2D::SetCirclePosition(m_CirclePosition);
                Renderer2D::SetCircleRotation(m_CircleRotation);
                Renderer2D::SetCircleRadius(m_CircleRadius);
                Renderer2D::SetCircleColor(glm::vec4(m_CircleColor[0], m_CircleColor[1], m_CircleColor[2], m_CircleColor[3]));
                Renderer2D::SetCircleThickness(m_CircleThickness);
                Renderer2D::SetCircleFade(m_CircleFade);
                Renderer2D::DrawCircle();
            }

            if (m_LineEnabled)
            {
                Renderer2D::SetLineStart(m_LineStart);
                Renderer2D::SetLineEnd(m_LineEnd);
                Renderer2D::SetLineThickness(m_LineThickness);
                Renderer2D::SetLineColor(glm::vec4(m_LineColor[0], m_LineColor[1], m_LineColor[2], m_LineColor[3]));
                Renderer2D::DrawLine();
            }

            if (m_TextEnabled)
            {
                Renderer2D::SetStringContent(std::string(m_TextContent));
                Renderer2D::SetStringPosition(m_TextPosition);
                Renderer2D::SetStringColor(glm::vec4(m_TextColor[0], m_TextColor[1], m_TextColor[2], m_TextColor[3]));
                Renderer2D::SetStringSize(m_TextSize);
                Renderer2D::DrawString();
            }

            if (m_PixelEnabled)
            {

                Renderer2D::SetPixelSize(m_PixelSize);
                Renderer2D::SetPixelColor(glm::vec4(m_PixelColor[0], m_PixelColor[1], m_PixelColor[2], m_PixelColor[3]));
                for (int i = 0; i < m_PixelCount; ++i)
                {
                    glm::vec3 pixelPos = m_PixelPosition + glm::vec3(
                        (rand() % 200 - 100) * 0.1f,
                        (rand() % 200 - 100) * 0.1f,
                        0.0f
                    );
                    Renderer2D::SetPixelPosition(pixelPos);
                    Renderer2D::DrawPixel();
                }
            }

            if (m_TriangleEnabled)
            {
                Renderer2D::SetTrianglePoint1(m_TrianglePoint1);
                Renderer2D::SetTrianglePoint2(m_TrianglePoint2);
                Renderer2D::SetTrianglePoint3(m_TrianglePoint3);
                Renderer2D::SetTriangleColor(glm::vec4(m_TriangleColor[0], m_TriangleColor[1], m_TriangleColor[2], m_TriangleColor[3]));
                Renderer2D::DrawTriangle();
            }

            if (m_GridEnabled)
            {
                Renderer2D::SetGridPosition(m_GridPosition);
                Renderer2D::SetGridRotation(m_GridRotation);
                Renderer2D::SetGridSize(m_GridSize);
                Renderer2D::SetGridCellSize(m_GridCellSize);
                Renderer2D::SetGridColor(glm::vec4(m_GridColor[0], m_GridColor[1], m_GridColor[2], m_GridColor[3]));
                Renderer2D::SetGridLineWidth(m_GridLineWidth);
                Renderer2D::SetGridShowCheckerboard(m_GridShowCheckerboard);
                Renderer2D::SetGridCheckerColor1(glm::vec4(m_GridCheckerColor1[0], m_GridCheckerColor1[1], m_GridCheckerColor1[2], m_GridCheckerColor1[3]));
                Renderer2D::SetGridCheckerColor2(glm::vec4(m_GridCheckerColor2[0], m_GridCheckerColor2[1], m_GridCheckerColor2[2], m_GridCheckerColor2[3]));
                Renderer2D::DrawGrid();
            }

            Renderer2D::End();

            ImGui::Image((void*)(intptr_t)Renderer2D::GetImage(), { size.x, size.y });
            ImGui::End();

            // Controls panel
            RenderControlsPanel();

            // Statistics panel
            RenderStatsPanel();
        }

    private:
        void RenderControlsPanel()
        {
            ImGui::Begin("Renderer Controls");

            if (ImGui::CollapsingHeader("Quad", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Enable Quad", &m_QuadEnabled);
                if (m_QuadEnabled)
                {
                    ImGui::SliderFloat3("Position##Quad", &m_QuadPosition.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("Rotation##Quad", &m_QuadRotation.x, -180.0f, 180.0f);
                    ImGui::SliderFloat2("Size##Quad", &m_QuadSize.x, 0.1f, 5.0f);
                    ImGui::ColorEdit4("Color##Quad", m_QuadColor);

                    if (ImGui::Button("Reset Quad"))
                    {
                        m_QuadPosition = { 0.0f, 0.0f, 0.0f };
                        m_QuadRotation = { 0.0f, 0.0f, 0.0f };
                        m_QuadSize = { 1.0f, 1.0f };
                        m_QuadColor[0] = 1.0f; m_QuadColor[1] = 0.0f; m_QuadColor[2] = 0.0f; m_QuadColor[3] = 1.0f;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Circle"))
            {
                ImGui::Checkbox("Enable Circle", &m_CircleEnabled);
                if (m_CircleEnabled)
                {
                    ImGui::SliderFloat3("Position##Circle", &m_CirclePosition.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("Rotation##Circle", &m_CircleRotation.x, -180.0f, 180.0f);
                    ImGui::SliderFloat2("Radius##Circle", &m_CircleRadius.x, 0.1f, 5.0f);
                    ImGui::SliderFloat("Thickness##Circle", &m_CircleThickness, 0.0f, 1.0f);
                    ImGui::SliderFloat("Fade##Circle", &m_CircleFade, 0.0f, 1.0f);
                    ImGui::ColorEdit4("Color##Circle", m_CircleColor);

                    if (ImGui::Button("Reset Circle"))
                    {
                        m_CirclePosition = { 2.0f, 0.0f, 0.0f };
                        m_CircleRotation = { 0.0f, 0.0f, 0.0f };
                        m_CircleRadius = { 1.0f, 1.0f };
                        m_CircleThickness = 1.0f;
                        m_CircleFade = 0.0f;
                        m_CircleColor[0] = 0.0f; m_CircleColor[1] = 1.0f; m_CircleColor[2] = 0.0f; m_CircleColor[3] = 1.0f;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Line"))
            {
                ImGui::Checkbox("Enable Line", &m_LineEnabled);
                if (m_LineEnabled)
                {
                    ImGui::SliderFloat3("Start##Line", &m_LineStart.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("End##Line", &m_LineEnd.x, -10.0f, 10.0f);
                    ImGui::SliderFloat("Thickness##Line", &m_LineThickness, 1.0f, 10.0f);
                    ImGui::ColorEdit4("Color##Line", m_LineColor);

                    if (ImGui::Button("Reset Line"))
                    {
                        m_LineStart = { -2.0f, -2.0f, 0.0f };
                        m_LineEnd = { 2.0f, 2.0f, 0.0f };
                        m_LineThickness = 2.0f;
                        m_LineColor[0] = 1.0f; m_LineColor[1] = 1.0f; m_LineColor[2] = 0.0f; m_LineColor[3] = 1.0f;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Text"))
            {
                ImGui::Checkbox("Enable Text", &m_TextEnabled);
                if (m_TextEnabled)
                {
                    ImGui::InputText("Content##Text", m_TextContent, sizeof(m_TextContent));
                    ImGui::SliderFloat3("Position##Text", &m_TextPosition.x, -10.0f, 10.0f);
                    ImGui::SliderFloat("Size##Text", &m_TextSize, 0.1f, 2.0f);
                    ImGui::ColorEdit4("Color##Text", m_TextColor);

                    if (ImGui::Button("Reset Text"))
                    {
                        m_TextContent[0] = '\0';
                        m_TextPosition = { -3.0f, 3.0f, 0.0f };
                        m_TextSize = 0.5f;
                        m_TextColor[0] = 1.0f; m_TextColor[1] = 1.0f; m_TextColor[2] = 1.0f; m_TextColor[3] = 1.0f;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Pixels"))
            {
                ImGui::Checkbox("Enable Pixels", &m_PixelEnabled);
                if (m_PixelEnabled)
                {
                    ImGui::SliderFloat3("Center Position##Pixel", &m_PixelPosition.x, -10.0f, 10.0f);
					ImGui::SliderFloat("Size##Pixel", &m_PixelSize, 0.1f, 5.0f);
                    ImGui::SliderInt("Count##Pixel", &m_PixelCount, 1, 1000);
                    ImGui::ColorEdit4("Color##Pixel", m_PixelColor);

                    if (ImGui::Button("Reset Pixels"))
                    {
                        m_PixelPosition = { 0.0f, -3.0f, 0.0f };
                        m_PixelCount = 100;
                        m_PixelColor[0] = 1.0f; m_PixelColor[1] = 0.0f; m_PixelColor[2] = 1.0f; m_PixelColor[3] = 1.0f;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Triangle"))
            {
                ImGui::Checkbox("Enable Triangle", &m_TriangleEnabled);
                if (m_TriangleEnabled)
                {
                    ImGui::SliderFloat3("Point 1##Triangle", &m_TrianglePoint1.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("Point 2##Triangle", &m_TrianglePoint2.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("Point 3##Triangle", &m_TrianglePoint3.x, -10.0f, 10.0f);
                    ImGui::ColorEdit4("Color##Triangle", m_TriangleColor);

                    if (ImGui::Button("Reset Triangle"))
                    {
                        m_TrianglePoint1 = { -1.0f, -1.0f, 0.0f };
                        m_TrianglePoint2 = { 1.0f, -1.0f, 0.0f };
                        m_TrianglePoint3 = { 0.0f, 1.0f, 0.0f };
                        m_TriangleColor[0] = 0.0f; m_TriangleColor[1] = 0.0f; m_TriangleColor[2] = 1.0f; m_TriangleColor[3] = 1.0f;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Grid"))
            {
                ImGui::Checkbox("Enable Grid", &m_GridEnabled);
                if (m_GridEnabled)
                {
                    ImGui::SliderFloat3("Position##Grid", &m_GridPosition.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("Rotation##Grid", &m_GridRotation.x, -180.0f, 180.0f);
                    ImGui::SliderFloat2("Size##Grid", &m_GridSize.x, 1.0f, 40.0f);
                    ImGui::SliderFloat("Cell Size##Grid", &m_GridCellSize, 0.1f, 2.0f);
                    ImGui::SliderFloat("Line Width##Grid", &m_GridLineWidth, 0.5f, 5.0f);
                    ImGui::Checkbox("Show Checkerboard##Grid", &m_GridShowCheckerboard);
                    ImGui::ColorEdit4("Grid Color##Grid", m_GridColor);
                    if (m_GridShowCheckerboard)
                    {
                        ImGui::ColorEdit4("Checker Color 1##Grid", m_GridCheckerColor1);
                        ImGui::ColorEdit4("Checker Color 2##Grid", m_GridCheckerColor2);
                    }

                    if (ImGui::Button("Reset Grid"))
                    {
                        m_GridPosition = { 0.0f, 0.0f, -1.0f };
                        m_GridRotation = { 0.0f, 0.0f, 0.0f };
                        m_GridSize = { 10.0f, 10.0f };
                        m_GridCellSize = 1.0f;
                        m_GridLineWidth = 1.0f;
                        m_GridShowCheckerboard = true;
                        m_GridColor[0] = 0.3f; m_GridColor[1] = 0.3f; m_GridColor[2] = 0.3f; m_GridColor[3] = 0.8f;
                        m_GridCheckerColor1[0] = 0.9f; m_GridCheckerColor1[1] = 0.9f; m_GridCheckerColor1[2] = 0.9f; m_GridCheckerColor1[3] = 0.2f;
                        m_GridCheckerColor2[0] = 0.8f; m_GridCheckerColor2[1] = 0.8f; m_GridCheckerColor2[2] = 0.8f; m_GridCheckerColor2[3] = 0.2f;
                    }
                }
            }

            ImGui::Separator();
            if (ImGui::Button("Reset All"))
            {
                ResetAllObjects();
            }

            if (ImGui::Button("Enable All"))
            {
                m_QuadEnabled = m_CircleEnabled = m_LineEnabled = m_TextEnabled =
                    m_PixelEnabled = m_TriangleEnabled = m_GridEnabled = true;
            }

            if (ImGui::Button("Disable All"))
            {
                m_QuadEnabled = m_CircleEnabled = m_LineEnabled = m_TextEnabled =
                    m_PixelEnabled = m_TriangleEnabled = m_GridEnabled = false;
            }

            ImGui::End();
        }

        void RenderStatsPanel()
        {
            ImGui::Begin("Renderer Statistics");

            auto stats = Renderer2D::GetStats();

            ImGui::Text("Performance:");
            ImGui::Text("FPS: %.2f", m_FPS);
            ImGui::Text("Draw Calls: %u", stats.DrawCalls);
            ImGui::Text("Data Size: %u bytes", stats.DataSize);
            ImGui::Text("Textures Used: %u", stats.TexturesUsed);

            ImGui::Separator();
            ImGui::Text("Geometry Count:");
            ImGui::Text("Quads: %u", stats.QuadCount);
            ImGui::Text("Circles: %u", stats.CircleCount);
            ImGui::Text("Lines: %u", stats.LineCount);
            ImGui::Text("Text: %u", stats.TextCount);
            ImGui::Text("Pixels: %u", stats.PixelCount);
            ImGui::Text("Triangles: %u", stats.TriangleCount);
            ImGui::Text("Grids: %u", stats.GridCount);

            ImGui::Separator();
            ImGui::Text("Total Vertices: %u", stats.GetTotalVertexCount());
            ImGui::Text("Total Indices: %u", stats.GetTotalIndexCount());

            ImGui::End();
        }

        void ResetAllObjects()
        {
            m_QuadPosition = { 0.0f, 0.0f, 0.0f };
            m_QuadRotation = { 0.0f, 0.0f, 0.0f };
            m_QuadSize = { 1.0f, 1.0f };
            m_QuadColor[0] = 1.0f; m_QuadColor[1] = 0.0f; m_QuadColor[2] = 0.0f; m_QuadColor[3] = 1.0f;

            m_CirclePosition = { 2.0f, 0.0f, 0.0f };
            m_CircleRotation = { 0.0f, 0.0f, 0.0f };
            m_CircleRadius = { 1.0f, 1.0f };
            m_CircleThickness = 1.0f;
            m_CircleFade = 0.0f;
            m_CircleColor[0] = 0.0f; m_CircleColor[1] = 1.0f; m_CircleColor[2] = 0.0f; m_CircleColor[3] = 1.0f;

            m_LineStart = { -2.0f, -2.0f, 0.0f };
            m_LineEnd = { 2.0f, 2.0f, 0.0f };
            m_LineThickness = 2.0f;
            m_LineColor[0] = 1.0f; m_LineColor[1] = 1.0f; m_LineColor[2] = 0.0f; m_LineColor[3] = 1.0f;

            m_TextContent[0] = '\0';
            m_TextPosition = { -3.0f, 3.0f, 0.0f };
            m_TextSize = 0.5f;
            m_TextColor[0] = 1.0f; m_TextColor[1] = 1.0f; m_TextColor[2] = 1.0f; m_TextColor[3] = 1.0f;

            m_PixelPosition = { 0.0f, -3.0f, 0.0f };
			m_PixelSize = 1.0f;
            m_PixelCount = 100;
            m_PixelColor[0] = 1.0f; m_PixelColor[1] = 0.0f; m_PixelColor[2] = 1.0f; m_PixelColor[3] = 1.0f;

            m_TrianglePoint1 = { -1.0f, -1.0f, 0.0f };
            m_TrianglePoint2 = { 1.0f, -1.0f, 0.0f };
            m_TrianglePoint3 = { 0.0f, 1.0f, 0.0f };
            m_TriangleColor[0] = 0.0f; m_TriangleColor[1] = 0.0f; m_TriangleColor[2] = 1.0f; m_TriangleColor[3] = 1.0f;

            m_GridPosition = { 0.0f, 0.0f, -1.0f };
            m_GridRotation = { 0.0f, 0.0f, 0.0f };
            m_GridSize = { 20.0f, 20.0f };
            m_GridCellSize = 1.0f;
            m_GridLineWidth = 1.0f;
            m_GridShowCheckerboard = true;
            m_GridColor[0] = 0.3f; m_GridColor[1] = 0.3f; m_GridColor[2] = 0.3f; m_GridColor[3] = 0.8f;
            m_GridCheckerColor1[0] = 0.9f; m_GridCheckerColor1[1] = 0.9f; m_GridCheckerColor1[2] = 0.9f; m_GridCheckerColor1[3] = 0.2f;
            m_GridCheckerColor2[0] = 0.8f; m_GridCheckerColor2[1] = 0.8f; m_GridCheckerColor2[2] = 0.8f; m_GridCheckerColor2[3] = 0.2f;
        }

    private:
        Ref<OrthographicCamera> m_OrthographicCamera;
        Timer m_FrameTimer;
        float m_FPS = 0.0f;

		Ref<TextureAtlas> m_TextureAtlas;

        // Quad properties
        bool m_QuadEnabled = true;
        glm::vec3 m_QuadPosition = { -3.0f, 0.0f, 0.0f };
        glm::vec3 m_QuadRotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 m_QuadSize = { 1.0f, 1.0f };
        float m_QuadColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        // Circle properties
        bool m_CircleEnabled = true;
        glm::vec3 m_CirclePosition = { 2.0f, 0.0f, 0.0f };
        glm::vec3 m_CircleRotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 m_CircleRadius = { 1.0f, 1.0f };
        float m_CircleThickness = 1.0f;
        float m_CircleFade = 0.0f;
        float m_CircleColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

        // Line properties
        bool m_LineEnabled = true;
        glm::vec3 m_LineStart = { -2.0f, -2.0f, 0.0f };
        glm::vec3 m_LineEnd = { 2.0f, 2.0f, 0.0f };
        float m_LineThickness = 2.0f;
        float m_LineColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };

        // Text properties
        bool m_TextEnabled = true;
        char m_TextContent[256] = "Hello World!";
        glm::vec3 m_TextPosition = { -3.0f, 3.0f, 0.0f };
        float m_TextSize = 0.5f;
        float m_TextColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

        // Pixel properties
        bool m_PixelEnabled = true;
        glm::vec3 m_PixelPosition = { 0.0f, -3.0f, 0.0f };
		float m_PixelSize = 1.0f;
        int m_PixelCount = 100;
        float m_PixelColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };

        // Triangle properties
        bool m_TriangleEnabled = true;
        glm::vec3 m_TrianglePoint1 = { -1.0f, -1.0f, 0.0f };
        glm::vec3 m_TrianglePoint2 = { 1.0f, -1.0f, 0.0f };
        glm::vec3 m_TrianglePoint3 = { 0.0f, 1.0f, 0.0f };
        float m_TriangleColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

        // Grid properties
        bool m_GridEnabled = true;
        glm::vec3 m_GridPosition = { 0.0f, 0.0f, -1.0f };
        glm::vec3 m_GridRotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 m_GridSize = { 20.0f, 20.0f };
        float m_GridCellSize = 1.0f;
        float m_GridLineWidth = 1.0f;
        bool m_GridShowCheckerboard = true;
        float m_GridColor[4] = { 0.3f, 0.3f, 0.3f, 0.8f };
        float m_GridCheckerColor1[4] = { 0.9f, 0.9f, 0.9f, 0.2f };
        float m_GridCheckerColor2[4] = { 0.8f, 0.8f, 0.8f, 0.2f };
    };
}