#include "ECS/RenderComponent.h"
#include "ECS/Entity.h"

namespace Enigma {
	namespace ECS {
		glm::vec2 OrthographicCamera::ScreenPosition(const glm::vec3& worldPos)
		{
			auto transform = ECS::Entity::Get(m_ParentID)->GetComponent<Transform>();
			Renderer::OrthographicCamera* camera = new Renderer::OrthographicCamera(m_ViewBox, m_Zoom, transform->GetPosition());

			glm::vec2 rslt = camera->WorldToScreen(worldPos);

			delete camera;

			return rslt;
		}
		glm::vec3 OrthographicCamera::WorldPosition(const glm::vec2& screenPos)
		{
			auto transform = ECS::Entity::Get(m_ParentID)->GetComponent<Transform>();
			Renderer::OrthographicCamera* camera = new Renderer::OrthographicCamera(m_ViewBox, m_Zoom, transform->GetPosition());
			
			glm::vec3 rslt = camera->ScreenToWorld(screenPos);

			delete camera;

			return rslt;
		}
	}
}