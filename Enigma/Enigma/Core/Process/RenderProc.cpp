#include "Core/Process/RenderProc.h"

#include "Renderer/Renderer2D.h"
#include "ECS/RenderComponment.h"
#include "ECS/Entity.h"

namespace Enigma {
	namespace Core {

		void RenderProc::StartUp()
		{
			m_ActiveCamera = nullptr;
			m_SearchCounter = 5;
		}
		void RenderProc::ShutDown()
		{
		}

		bool RenderProc::OnEvent(Event& e)
		{
			return false;
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

				if (m_ActiveCamera == nullptr) {
					m_ActiveCamera = new Renderer::OrthographicCamera(Renderer::ViewBox::ScreenViewBox());
				}

				m_ActiveCamera->SetPosition(transform->GetPosition());

				found = true;
				break;
			}
			if (!found) {
				delete m_ActiveCamera;
				m_ActiveCamera = nullptr;
			}
		}

		void RenderProc::Render()
		{
			if (m_ActiveCamera == nullptr) return;

			Renderer::Render2D::StartFrame(m_ActiveCamera);

			for (auto& comp : ECS::Render2D::GetList()) {
				auto transform = ECS::Entity::Get(comp->GetParentID())->GetComponent<ECS::Transform>();
				if (comp->GetTexture() != nullptr) {
					Renderer::Render2D::DrawQuad(
						transform->GetPosition(),
						transform->GetScale(),
						glm::radians(transform->GetRotation().z),
						comp->GetTexture(),
						comp->GetTint()
					);
					continue;
				}
				Renderer::Render2D::DrawQuad(
					transform->GetPosition(),
					transform->GetScale(),
					glm::radians(transform->GetRotation().z),
					comp->GetTint()
				);
			}

			Renderer::Render2D::EndFrame();
		}

	}
}