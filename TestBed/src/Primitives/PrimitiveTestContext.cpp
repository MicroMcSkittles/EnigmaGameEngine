#include "Primitives/PrimitiveTestContext.h"
#include <Enigma/Core/Process/Application.h>
#include <EnigmaSerialization/Image.h>
#include <imgui.h>

PrimitiveTestContext::PrimitiveTestContext(Core::ID windowID) : TestContext(windowID)
{
	Core::Window* window = Core::Application::GetWindow(m_WindowID);

	m_Surface.scale = {
		window->GetWidth(),
		window->GetHeight()
	};
	
	Renderer::Render2DConfig renderConfig;
	renderConfig.renderAPI = window->GetAPI();
	renderConfig.surface = m_Surface;
	Core::Application::UseRenderAPI(window->GetAPI());
	Renderer::ShaderConfig mainShaderConfig;
	mainShaderConfig.vertexPath = "assets/SceneViewShader.vert";
	mainShaderConfig.pixelPath = "assets/SceneViewShader.frag";
	renderConfig.mainShader = Renderer::Shader::Create(mainShaderConfig);

	m_RenderContext = new Renderer::Render2D(renderConfig);

	Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(m_Surface);
	m_Camera = new Renderer::OrthographicCamera(viewBox, 2);

	m_Position = { -0.1,-0.6 };
	m_Radius = 1;
	m_Tint = { 1,1,1,1 };

	Serialization::ImageConfig textureConfig;
	m_Texture = Serialization::ImageLoader::Load("assets/test.png", textureConfig);
}

void PrimitiveTestContext::Render()
{
	m_RenderContext->StartFrame(m_Camera);
	m_RenderContext->DrawQuad({ 0,-1.5 }, { 3,0.25 }, 0, -1, { 1,1,1,1 });
	m_RenderContext->DrawCircle(m_Position, m_Radius, -1, m_Texture, m_Tint);
	m_RenderContext->DrawCircle({ 1,0 }, 0.5, 0, { 1,0,1,1 });
	m_RenderContext->EndFrame();
}
void PrimitiveTestContext::ImGui()
{
	ImGui::Begin("Debug Menu", 0, ImGuiWindowFlags_NoTitleBar);
	ImGui::SeparatorText("Primitive Test Menu");

	ImGui::DragFloat2("Position", &m_Position.x, 0.1f);
	ImGui::DragFloat("Radius", &m_Radius, 0.1f);
	ImGui::ColorEdit4("Tint", &m_Tint.r);

	ImGui::End();
}

void PrimitiveTestContext::OnResize(Core::WindowResize& e)
{
	m_RenderContext->Resize(e.GetWidth(), e.GetHeight());
}
void PrimitiveTestContext::OnEvent(Core::Event& e)
{
	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowResize>([&](Core::WindowResize& e) { OnResize(e); return false; });
}