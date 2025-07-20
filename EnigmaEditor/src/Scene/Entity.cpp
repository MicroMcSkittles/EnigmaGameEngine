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

		EntityInspectorContext::EntityInspectorContext(Entity* entity) 
			: m_Entity(entity), m_ComponentPopup(false)
		{
		}
		EntityInspectorContext::~EntityInspectorContext()
		{
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

			if (ImGui::BeginPopup("CreateComponentPopup")) CreateComponentPopup();

			for (auto& [type, id] : m_Entity->compoentIDs) {
				EditorComponentInterface* component = m_Entity->scene->GetComponent(id);
				component->InspectorImGui();
				ImGui::Spacing();
			}
		}

		void EntityInspectorContext::CreateComponentPopup()
		{
			ImGui::BeginChild("CreateComponentPopupChild", { 100, 50 }, ImGuiChildFlags_AutoResizeX);

			using namespace Engine::ECS;
			ECS::MakeCurrent(m_Entity->scene->GetECS());
			
			bool avalibleComponents = false;

			for (std::string& name : s_ComponentNames) {
				ComponentType type = s_ComponentTypeMap[name];
				// Skip component if the entity already has the component
				if (ECS::GetEntity(m_Entity->entityID).components.count(type)) continue;

				avalibleComponents = true;

				if (ImGui::MenuItem(name.c_str())) {
					switch (type)
					{
					case Enigma::Engine::ECS::ComponentType::Tag: m_Entity->scene->AddComponent<EditorTag>(m_Entity->ID); break;
					case Enigma::Engine::ECS::ComponentType::Transform: m_Entity->scene->AddComponent<EditorTransform>(m_Entity->ID); break;
					case Enigma::Engine::ECS::ComponentType::Render2D: m_Entity->scene->AddComponent<EditorRender2D>(m_Entity->ID); break;
					case Enigma::Engine::ECS::ComponentType::Camera: break;
					}
					LOG_MESSAGE("Created %s Component for %s", 5, name.c_str(), m_Entity->GetPathName().c_str());
					ImGui::CloseCurrentPopup();
					break;
				}
			}

			if (!avalibleComponents) ImGui::TextWrapped("No more avalible components");

			ImGui::EndChild();
			ImGui::EndPopup();
		}

	}
}