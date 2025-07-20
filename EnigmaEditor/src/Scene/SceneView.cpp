#include "Scene/SceneView.h"

#include <Enigma/Core/Process/Application.h>
#include <Enigma/Engine/ECS/RenderComponent.h>
#include <Enigma/Engine/Input.h>
#include <Enigma/Engine/InputCodes.h>

namespace Enigma {
	namespace Editor {
		void SceneView2D::StartUp()
		{
			Renderer::ViewBox viewBox;
			viewBox.left = -1;
			viewBox.right = 1;
			viewBox.bottom = -1;
			viewBox.top = 1;
			viewBox.near = 0.1;
			viewBox.far = 1000;
			m_Camera = new Renderer::OrthographicCamera(viewBox);
			m_Focused = false;
			m_ZoomSpeed = 4.0f;
			m_CameraSpeed = 0.25f;
		}
		void SceneView2D::ShutDown()
		{
			if(m_Camera) delete m_Camera;
			if(m_InputContext) delete m_InputContext;
			if(m_RendererContext) delete m_RendererContext;
		}

		bool SceneView2D::OnMouseScroll(Core::MouseScroll& e)
		{
			if (!m_Focused) return false;

			float zoom = m_Camera->GetZoom();
			zoom -= e.GetY() / m_ZoomSpeed;
			if (zoom < 0.1f) zoom = 0.1f;
			else if (zoom > 10) zoom = 10;
			m_Camera->SetZoom(zoom);

			return false;
		}
		bool SceneView2D::OnEvent(Core::Event& e)
		{
			Core::EventHandler handler(e);
			handler.Dispatch<Core::MouseScroll>([&](Core::MouseScroll& e) { return OnMouseScroll(e); });
			return false;
		}
		void SceneView2D::Update(Enigma::Engine::DeltaTime deltaTime)
		{
			Engine::Input::MakeCurrent(m_InputContext);

			glm::vec2 mousePos = Engine::Input::GetMousePosition();
			m_Focused = (mousePos.x >= 0 && mousePos.x < m_Surface.scale.x) && (mousePos.y >= 0 && mousePos.y < m_Surface.scale.y);
			m_Focused = Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonRight) && m_Focused;
		
			if (!m_Focused) return;

			glm::vec2 move = { 0,0 };
			if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyW)) move.y = 1;
			if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyS)) move.y = -1;
			if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyA)) move.x = -1;
			if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyD)) move.x = 1;
			if (move.x != 0 || move.y != 0) {
				glm::vec2 dir = glm::normalize(move);
				glm::vec3 position = m_Camera->GetPosition();
				position += glm::vec3(dir * m_CameraSpeed * (m_Camera->GetZoom() / 4.0f), 0);
				m_Camera->SetPosition(position);
			}
		}

		void SceneView2D::Render()
		{
			using namespace Engine::ECS;
			ECS::MakeCurrent(m_SceneContext->GetECS());
			ComponentPool<Engine::ECS::Render2D>* pool = ECS::GetPool<Render2D>();
			if (pool == nullptr) return;

			Renderer::Renderer::MakeCurrent(m_RendererContext);
			Renderer::Render2D::StartFrame(m_Camera);

			for (Render2D& renderComp : pool->GetData()) {
				Transform transform = ECS::GetComponent<Transform>(renderComp.parentEntity).ApplyParent();

				if (renderComp.texture == nullptr) {
					Renderer::Render2D::DrawQuad(
						transform.position,
						transform.scale,
						transform.rotation.z,
						renderComp.depth,
						renderComp.tint
					);
				}
				else {
					Renderer::Render2D::DrawQuad(
						transform.position,
						transform.scale,
						transform.rotation.z,
						renderComp.depth,
						renderComp.texture,
						renderComp.tint
					);
				}
			}

			Renderer::Render2D::EndFrame();
		}

		void SceneView2D::SetWindowContext(Core::ID window)
		{
			m_Window = Core::Application::GetWindow(window);
			Core::Application::BindSubProcToWindow(this, window);
			m_Window->AddEventCallback([&](Core::Event& e) { OnEvent(e); });

			Core::Application::UseRenderAPI(m_Window->GetAPI());

			Renderer::TextureConfig frameConfig;
			frameConfig.width = 400;
			frameConfig.height = 400;
			m_Surface.frame = Renderer::Texture::Create(frameConfig);
			m_Surface.scale = { 400, 400 };

			Engine::InputConfig inputConfig;
			inputConfig.surface = &m_Surface;
			inputConfig.window = m_Window;
			m_InputContext = Engine::Input::Create(inputConfig);

			Renderer::Render2DConfig renderConfig;
			renderConfig.renderAPI = m_Window->GetAPI();
			renderConfig.surface = m_Surface;

			Renderer::ShaderConfig shaderConfig;
			shaderConfig.vertexPath = "ExampleProject/.Enigma/DefaultShaders/SceneViewShader.vert";
			shaderConfig.pixelPath = "ExampleProject/.Enigma/DefaultShaders/SceneViewShader.frag";
			renderConfig.mainShader = Renderer::Shader::Create(shaderConfig);

			shaderConfig.vertexPath = "ExampleProject/.Enigma/DefaultShaders/PostProcessShader.vert";
			shaderConfig.pixelPath = "ExampleProject/.Enigma/DefaultShaders/PostProcessShader.frag";
			renderConfig.postProcShader = Renderer::Shader::Create(shaderConfig);

			if (m_RendererContext) delete m_RendererContext;
			m_RendererContext = new Renderer::Render2D(renderConfig);
		}
		void SceneView2D::SetSurface(const Engine::Surface& surface)
		{
			m_Surface.position = surface.position;
			m_Surface.scale = surface.scale;
		}

		void SceneView2D::Resize(int width, int height)
		{
			m_Surface.Resize(width, height);

			if (!m_RendererContext) return;
			if (m_Camera) m_Camera->Resize(width, height);
			Renderer::Renderer::MakeCurrent(m_RendererContext);
			m_RendererContext->Resize(width, height);
		}

		Renderer::Texture* SceneView2D::GetFrame()
		{
			return m_Surface.frame;
		}
	}
}