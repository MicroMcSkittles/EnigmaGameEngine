#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Enigma {
	namespace Editor {
		TagContext::TagContext(Entity* parent) : m_Parent(parent)
		{
			Engine::ECS::ECS::MakeCurrent(m_Parent->scene->GetECS());
			m_Component = &Engine::ECS::ECS::GetComponent<Engine::ECS::Tag>(m_Parent->entityID);
		}
		void TagContext::ImGui()
		{
			std::string strID = m_Parent->GetPathName() + " - Tag";
			//if (!ImGui::TreeNode(strID.c_str(), "Tag")) return;

			ImGui::SeparatorText("Tag");
			ImGui::InputText("Tag", &m_Component->tag);

			//ImGui::TreePop();
		}


		TransformContext::TransformContext(Entity* parent) : m_Parent(parent)
		{
			Engine::ECS::ECS::MakeCurrent(m_Parent->scene->GetECS());
			m_Component = &Engine::ECS::ECS::GetComponent<Engine::ECS::Transform>(m_Parent->entityID);
		}
		void TransformContext::ImGui()
		{
			std::string strID = m_Parent->GetPathName() + " - Transform";
			//if (!ImGui::TreeNode(strID.c_str(), "Transform")) return;

			ImGui::SeparatorText("Transform");
			ImGui::InputFloat3("Position", &m_Component->position.x);
			ImGui::InputFloat3("Rotation", &m_Component->rotation.x);
			ImGui::InputFloat3("Scale", &m_Component->scale.x);

			//ImGui::TreePop();
		}

		Render2DContext::Render2DContext(Entity* parent) : m_Parent(parent)
		{
			Engine::ECS::ECS::MakeCurrent(m_Parent->scene->GetECS());
			m_Component = &Engine::ECS::ECS::GetComponent<Engine::ECS::Render2D>(m_Parent->entityID);
		}

		void Render2DContext::ImGui()
		{
			std::string strID = m_Parent->GetPathName() + " - Render 2D";
			//if (!ImGui::TreeNode(strID.c_str(), "Render 2D")) return;

			ImGui::SeparatorText("Render 2D");
			ImGui::InputInt("Depth", &m_Component->depth);
			ImGui::InputFloat4("Tint", &m_Component->tint.x);
			ImGui::Text("TODO: Add a texture imgui widget");

			//ImGui::TreePop();
		}

	}
}