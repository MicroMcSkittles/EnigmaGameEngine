#include "Panel/HierarchyPanel.h"

#include <imgui.h>

namespace Enigma {
	namespace Editor {

		void HierarchyPanel::ImGui()
		{
			if (m_SceneContext == nullptr) return;
			m_IsMenuOpen = false;

			ImGui::Begin("Hierarchy");

			for (int i = 0; i < m_SceneContext->GetEntities().size(); ++i) {
				Entity* entity = m_SceneContext->GetEntities()[i];
				if (entity->parentID != Core::ID::InvalidID()) continue;
				EntityNode(entity);
			}

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !m_IsMenuOpen) {
				ImGui::OpenPopup("NULLPTR_ENTITY_RIGHT_CLICK_MENU");
			}
			if (ImGui::BeginPopup("NULLPTR_ENTITY_RIGHT_CLICK_MENU")) {
				RightClickMenu();
				ImGui::EndPopup();
			}

			ImGui::End();
		}

		void HierarchyPanel::DeleteEntity(Entity* entity)
		{
			LOG_MESSAGE("Delete Entity ( %s )", 5, entity->GetPathName().c_str());
			m_SceneContext->DeleteEntity(entity->ID);
			if (m_Selected == entity) m_SelectCallback(nullptr);
		}
		void HierarchyPanel::CreateChildEntity(Entity* parent)
		{
			LOG_MESSAGE("Create Child Entity For ( %s )", 5, parent->GetPathName().c_str());
			m_SceneContext->CreateEntity("New Entity", parent->entityID);
		}
		void HierarchyPanel::CreateEntity()
		{
			LOG_MESSAGE("Create Entity", 5);
			m_SceneContext->CreateEntity("New Entity");
		}

		void HierarchyPanel::EntityRightClickMenu(Entity* entity)
		{
			m_IsMenuOpen = true;
			const char* options[] = { "Delete Entity", "Create Child Entity" };
			for (int i = 0; i < IM_ARRAYSIZE(options); i++) {
				if (ImGui::Selectable(options[i])) {
					if (options[i] == "Delete Entity") {
						DeleteEntity(entity);
						
					}
					if (options[i] == "Create Child Entity") {
						CreateChildEntity(entity);
					}
				}
			}
		}
		void HierarchyPanel::RightClickMenu()
		{
			m_IsMenuOpen = true;
			const char* options[] = { "Create Entity" };
			for (int i = 0; i < IM_ARRAYSIZE(options); i++) {
				if (ImGui::Selectable(options[i])) {
					if (options[i] == "Create Entity") {
						CreateEntity();
					}
				}
			}
		}

		void HierarchyPanel::EntityNode(Entity* entity)
		{
			ImGuiTreeNodeFlags flags = 0;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= (m_Selected == entity) ? ImGuiTreeNodeFlags_Selected : 0;
			bool opened = ImGui::TreeNodeEx((void*)entity, flags, entity->name.c_str());

			if (ImGui::IsItemClicked()) {
				m_Selected = entity;
				m_SelectCallback(m_Selected);
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
				m_Selected = entity;
				m_SelectCallback(m_Selected);
				ImGui::OpenPopup("ENTITY_RIGHT_CLICK_MENU");
			}

			if (ImGui::BeginPopup("ENTITY_RIGHT_CLICK_MENU") ) {
				if (m_Selected == entity) EntityRightClickMenu(entity);
				ImGui::EndPopup();
			}

			if (!opened) return;

			for (Core::ID childID : entity->childrenIDs) {
				Entity* child = m_SceneContext->GetEntity(childID);
				EntityNode(child);
			}

			ImGui::TreePop();
		}

	}
}