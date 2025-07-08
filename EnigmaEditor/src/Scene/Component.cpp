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

		void SerializeTag(Engine::ECS::Tag& component, JSON::DataTreeNode& dataTree)
		{
			dataTree.value.type = JSON::DataTreeType::Object;
			dataTree["Type"] = "Tag";

			dataTree["Tag"] = component.tag;
		}
		void LoadTag(Engine::ECS::Tag& component, JSON::DataTreeNode& dataTree)
		{
			component.tag = dataTree["Tag"].value;
		}

		void SerializeTransform(Engine::ECS::Transform& component, JSON::DataTreeNode& dataTree)
		{
			dataTree.value.type = JSON::DataTreeType::Object;
			dataTree["Type"] = "Transform";

			dataTree["Position"].value.type = JSON::DataTreeType::Array;
			dataTree["Position"].elements.push_back(component.position.x);
			dataTree["Position"].elements.push_back(component.position.y);
			dataTree["Position"].elements.push_back(component.position.z);

			dataTree["Rotation"].value.type = JSON::DataTreeType::Array;
			dataTree["Rotation"].elements.push_back(component.rotation.x);
			dataTree["Rotation"].elements.push_back(component.rotation.y);
			dataTree["Rotation"].elements.push_back(component.rotation.z);

			dataTree["Scale"].value.type = JSON::DataTreeType::Array;
			dataTree["Scale"].elements.push_back(component.scale.x);
			dataTree["Scale"].elements.push_back(component.scale.y);
			dataTree["Scale"].elements.push_back(component.scale.z);
		}
		void LoadTransform(Engine::ECS::Transform& component, JSON::DataTreeNode& dataTree)
		{
			component.position.x = dataTree["Position"].elements[0].value;
			component.position.y = dataTree["Position"].elements[1].value;
			component.position.z = dataTree["Position"].elements[2].value;

			component.rotation.x = dataTree["Rotation"].elements[0].value;
			component.rotation.y = dataTree["Rotation"].elements[1].value;
			component.rotation.z = dataTree["Rotation"].elements[2].value;

			component.scale.x = dataTree["Scale"].elements[0].value;
			component.scale.y = dataTree["Scale"].elements[1].value;
			component.scale.z = dataTree["Scale"].elements[2].value;
		}

		void SerializeRender2D(Engine::ECS::Render2D& component, JSON::DataTreeNode& dataTree)
		{
			dataTree.value.type = JSON::DataTreeType::Object;
			dataTree["Type"] = "Render2D";

			dataTree["Depth"] = component.depth;
			
			dataTree["Tint"].value.type = JSON::DataTreeType::Array;
			dataTree["Tint"].elements.push_back(component.tint.x);
			dataTree["Tint"].elements.push_back(component.tint.y);
			dataTree["Tint"].elements.push_back(component.tint.z);
			dataTree["Tint"].elements.push_back(component.tint.w);

			dataTree["Texture"].value.type = JSON::DataTreeType::Null;
		}
		void LoadRender2D(Engine::ECS::Render2D& component, JSON::DataTreeNode& dataTree)
		{
			component.depth = dataTree["Depth"].value;

			component.tint.x = dataTree["Tint"].elements[0].value;
			component.tint.y = dataTree["Tint"].elements[1].value;
			component.tint.z = dataTree["Tint"].elements[2].value;
			component.tint.w = dataTree["Tint"].elements[3].value;

			component.texture = nullptr;
		}

	}
}