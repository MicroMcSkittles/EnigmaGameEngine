#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include <sstream>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Enigma {
	namespace Editor {

		std::string Entity::GetPathName()
		{
			std::stringstream ss;
			if (parentID != Core::ID::InvalidID()) {
				ss << scene->GetEntity(parentID)->GetPathName() << "/";
			}

			ss << name;

			return ss.str();
		}

		void Entity::Serialize(JSON::DataTreeNode& dataTree)
		{
			JSON::DataTreeNode node;
			node.value.type = JSON::DataTreeType::Object;

			node["Name"] = name;

			{
				using namespace Engine::ECS;
				ECS::MakeCurrent(scene->GetECS());
				JSON::DataTreeNode components;
				components.value.type = JSON::DataTreeType::Array;
				
				for (auto& [type, id] : ECS::GetEntity(entityID).components) {
					JSON::DataTreeNode component;
					switch (type)
					{
					case ComponentType::Tag: SerializeTag(ECS::GetPool<Tag>()->Get(id), component); break;
					case ComponentType::Transform: SerializeTransform(ECS::GetPool<Transform>()->Get(id), component); break;
					case ComponentType::Render2D: SerializeRender2D(ECS::GetPool<Render2D>()->Get(id), component); break;
					default: continue;
					}
					components.elements.push_back(component);
				}

				node["Components"] = components;
			}

			{
				JSON::DataTreeNode children;
				children.value.type = JSON::DataTreeType::Array;
				for (Core::ID childID : childrenIDs) {
					Entity* child = scene->GetEntity(childID);
					child->Serialize(children);
				}
				node["Children"] = children;
			}

			dataTree.elements.push_back(node);
		}

		EntityInspectorContext::EntityInspectorContext(Entity* entity) 
			: m_Entity(entity), m_ComponentPopup(false)
		{
			if (m_Entity == nullptr) return;

			using namespace Engine::ECS;
			
			for (auto& [type, id] : ECS::GetEntity(m_Entity->entityID).components) {
				switch (type)
				{
				case ComponentType::Tag: m_Components.push_back(new TagContext(m_Entity)); break;
				case ComponentType::Transform: m_Components.push_back(new TransformContext(m_Entity)); break;
				case ComponentType::Render2D: m_Components.push_back(new Render2DContext(m_Entity)); break;
				}
			}
		}
		EntityInspectorContext::~EntityInspectorContext()
		{
			m_Components.clear();
			if (m_Entity == nullptr) return;
			if (m_Entity->name.empty()) {
				m_Entity->name = "Unnamed Entity";
			}
		}

		void EntityInspectorContext::ImGui()
		{
			if (m_Entity == nullptr) return;
			ImGui::SeparatorText("Edit Entity");
			ImGui::InputText("Name", &m_Entity->name);

			ImGui::Text("Add Component");
			ImGui::SameLine();
			if (ImGui::Button("+")) ImGui::OpenPopup("CreateComponentPopup");

			if (ImGui::BeginPopupModal("CreateComponentPopup")) CreateComponentImGui();

			for (ComponentContext*& component : m_Components) {
				component->ImGui();
			}
		}

		void EntityInspectorContext::CreateComponentImGui()
		{
			ImGui::Text("Close");
			ImGui::SameLine();
			if (ImGui::Button("X")) ImGui::CloseCurrentPopup();

			ImGui::Text("Create component for ( %s )", m_Entity->GetPathName().c_str());

			static int component = 0;
			ImGui::Combo("Component Type", &component, s_ComponentNames, IM_ARRAYSIZE(s_ComponentNames));

			Engine::ECS::ComponentType type = s_ComponentTypeMap[s_ComponentNames[component]];

			Engine::ECS::ECS::MakeCurrent(m_Entity->scene->GetECS());
			if (Engine::ECS::ECS::GetEntity(m_Entity->entityID).components.count(type)) {
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 0, 0, 255));
				ImGui::Text("Entity ( %s ) already has a %s component.", m_Entity->GetPathName().c_str(), s_ComponentNames[component]);
				ImGui::PopStyleColor();
			}
			else {
				ImGui::Text("Create Component");
				ImGui::SameLine();
				if (ImGui::Button("+")) {
					switch (type)
					{
					case Enigma::Engine::ECS::ComponentType::Tag: 
						Engine::ECS::ECS::AddComponent<Engine::ECS::Tag>(m_Entity->entityID);
						m_Components.push_back(new TagContext(m_Entity));
						break;
					case Enigma::Engine::ECS::ComponentType::Transform:
						Engine::ECS::ECS::AddComponent<Engine::ECS::Transform>(m_Entity->entityID);
						m_Components.push_back(new TransformContext(m_Entity));
						break;
					case Enigma::Engine::ECS::ComponentType::Render2D:
						Engine::ECS::ECS::AddComponent<Engine::ECS::Render2D>(m_Entity->entityID);
						m_Components.push_back(new Render2DContext(m_Entity));
						break;
					case Enigma::Engine::ECS::ComponentType::Camera:
						break;
					}
					LOG_MESSAGE("Created %s Component for %s", 5, s_ComponentNames[component], m_Entity->GetPathName().c_str());
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

	}
}