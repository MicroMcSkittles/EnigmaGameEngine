#include "Core/Process/RenderProc.h"
#include "Core/Window.h"

#include "Renderer/Renderer2D.h"
#include "ECS/RenderComponent.h"
#include "ECS/Entity.h"


namespace Enigma {
	namespace Core {

		void RenderProc::StartUp()
		{
			m_ActiveCamera = nullptr;
			m_SearchCounter = 5;

			Renderer::Render2D::Init({});
		}
		void RenderProc::ShutDown()
		{
		}

		bool RenderProc::OnResize(Core::WindowResize& e) {
			Renderer::Render2D::Resize(e.GetWidth(), e.GetHeight());
			return false;
		}
		bool RenderProc::OnEvent(Event& e)
		{
			Core::EventHandler handler(e);
			handler.Dispatch<Core::WindowResize>(CLASS_BIND_ARGS_1(RenderProc::OnResize));
			return e.Handled();
		}

		void RenderProc::Update(float deltaTime)
		{
			// Used to find the current active camera every 5 frames
			if (m_SearchCounter != 5) {
				m_SearchCounter += 1;
				return;
			}
			m_SearchCounter = 0;

			// Find active camera
			bool found = false;
			for (auto& camera : ECS::Camera::GetList()) {
				if (!camera->GetIsActive()) continue;
				if (camera->GetType() == ECS::Camera::Type::Perspective) {
					LOG_ERROR("Perspective cameras are not supported yet!");
				}

				auto transform = ECS::Entity::Get(camera->GetParentID())->GetComponent<ECS::Transform>();
				auto ortho = (ECS::OrthographicCamera*)camera;

				if (m_ActiveCamera == nullptr)
					m_ActiveCamera = new Renderer::OrthographicCamera(ortho->GetViewBox());
				else ((Renderer::OrthographicCamera*)m_ActiveCamera)->SetViewBox(ortho->GetViewBox());

				((Renderer::OrthographicCamera*)m_ActiveCamera)->SetZoom(ortho->GetZoom());
				m_ActiveCamera->SetPosition(transform->GetPosition());

				auto window = Window::Get();
				m_ActiveCamera->Resize(window->GetWidth(), window->GetHeight());

				m_ActiveCameraEntity = ECS::Entity::Get(camera->GetParentID());
				found = true;
				break;
			}
			if (!found) {
				delete m_ActiveCamera;
				m_ActiveCamera = nullptr;
				m_ActiveCameraEntity = nullptr;
			}
		}

		void RenderProc::Render()
		{
			if (m_ActiveCamera == nullptr) return;

			auto transform = m_ActiveCameraEntity->GetComponent<ECS::Transform>();
			auto ortho = (ECS::OrthographicCamera*)m_ActiveCameraEntity->GetComponent<ECS::Camera>();

			((Renderer::OrthographicCamera*)m_ActiveCamera)->SetZoom(ortho->GetZoom());
			m_ActiveCamera->SetPosition(transform->GetPosition());

			Renderer::Render2D::StartFrame(m_ActiveCamera);

			for (auto& comp : ECS::Render2D::GetList()) {
				auto transform = ECS::Entity::Get(comp->GetParentID())->GetComponent<ECS::Transform>();
				if (comp->GetTexture() != nullptr) {
					Renderer::Render2D::DrawQuad(
						transform->GetPosition(),
						comp->GetDepth(),
						transform->GetScale(),
						transform->GetRotation().z,
						comp->GetTexture(),
						comp->GetTint()
					);
					continue;
				}
				Renderer::Render2D::DrawQuad(
					transform->GetPosition(),
					comp->GetDepth(),
					transform->GetScale(),
					transform->GetRotation().z,
					comp->GetTint()
				);
			}

			Renderer::Render2D::EndFrame();
		}

	}
}