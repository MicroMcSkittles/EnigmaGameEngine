#include "Engine/Render2DProc.h"
#include "Engine/Engine.h"
#include "Core/Process/Application.h"

#include "ECS/EntityComponentSystem.h"
#include "ECS/StanderedComponents.h"
#include "ECS/RenderComponents.h"

namespace Enigma {
	namespace Engine {

		void Render2DProc::StartUp()
		{
			Surface& surface = GetEngine()->GetSurface();
			Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(surface);
			m_CurrentCamera = new Renderer::OrthographicCamera(viewBox);
			m_LastFrameSize = surface.scale;
		}

		void Render2DProc::Render()
		{
			Surface& surface = GetEngine()->GetSurface();
			if (m_LastFrameSize != surface.scale) {
				m_LastFrameSize = surface.scale;
				Renderer::Render2D::Resize(surface.scale.x, surface.scale.y);
			}

			Renderer::Render2D::StartFrame(m_CurrentCamera);

			auto& renderComponents = ECS::ECS::GetComponentList<ECS::Render2D>();
			for (auto& comp : renderComponents) {
				ECS::Transform* transform = comp->GetParentEntity()->GetComponent<ECS::Transform>();
				if (comp->GetTexture() != nullptr) {
					Renderer::Render2D::DrawQuad(
						transform->GetPosition(),
						transform->GetScale(),
						transform->GetRotation().z,
						comp->GetDepth(),
						comp->GetTexture(),
						comp->GetTint()
					);
				}
				else {
					Renderer::Render2D::DrawQuad(
						transform->GetPosition(),
						transform->GetScale(),
						transform->GetRotation().z,
						comp->GetDepth(),
						comp->GetTint()
					);
				}
			}

			Renderer::Render2D::EndFrame();
		}
	}
}