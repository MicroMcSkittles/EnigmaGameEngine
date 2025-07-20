#include "Panel/HierarchyPanel.h"
#include "EditorWidgets.h"
#include "EditorEvent.h"

#include <Enigma/Core/Process/Application.h>
#include <imgui.h>

namespace Enigma {
	namespace Editor {

		void HierarchyPanel::ImGui()
		{
			if (m_SceneContext == nullptr) return;
			m_IsMenuOpen = false;

			ImGui::Begin("Hierarchy");
			ImGui::BeginChild("HIERARCHY_CHILD_WINDOW"); // Used for the DragDrop target
			ImGui::SeparatorText(m_SceneContext->GetName().c_str());

			// Show entity tree
			for (int i = 0; i < m_SceneContext->GetEntities().size(); ++i) {
				Entity* entity = m_SceneContext->GetEntities()[i];
				if (entity->parentID != Core::ID::InvalidID()) continue;
				EntityNode(entity);
			}

			// Right click menu stuff
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !m_IsMenuOpen) {
				ImGui::OpenPopup("NULLPTR_ENTITY_RIGHT_CLICK_MENU");
			}
			if (ImGui::BeginPopup("NULLPTR_ENTITY_RIGHT_CLICK_MENU")) {
				RightClickMenu();
				ImGui::EndPopup();
			}

			ImGui::EndChild();

			// Handle draged entitys to scene root
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY_DRAG_SOURCE")) {
					if (payload->DataSize != 64) LOG_SOFT_ERROR("Invalid hierarchy entity drag payload");
					else {
						Entity* draged = (Entity*)payload->Data;
						m_SceneContext->SetEntityParent(draged->ID, Core::ID::InvalidID());
						LOG_MESSAGE("Draged %s to scene root", 5, draged->name.c_str());
					}
				}
				ImGui::EndDragDropTarget();
			}

			// Handle Short Cuts
			if (ImGui::Shortcut(ImGuiKey_N + ImGuiMod_Ctrl + ImGuiMod_Shift)) CreateEntity();

			ImGui::End();
		}

		void HierarchyPanel::DeleteEntity(Entity* entity)
		{
			LOG_MESSAGE("Delete Entity ( %s )", 5, entity->GetPathName().c_str());
			m_SceneContext->DeleteEntity(entity->ID);
			if (m_Selected == entity) {
				NewInspectorContext newContext(nullptr);
				Core::Application::EventCallback(newContext);
			}
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
			if (ImGui::MenuItem("Delete Entity", "DEL")) {
				DeleteEntity(entity);
			}
			if (ImGui::MenuItem("Create Child Entity", "CTRL+N")) {
				CreateChildEntity(entity);
			}
		}
		void HierarchyPanel::RightClickMenu()
		{
			m_IsMenuOpen = true;
			if (ImGui::MenuItem("Create Entity", "CTRL+SHIFT+N")) {
				CreateEntity();
			}
		}

		void HierarchyPanel::EntityNode(Entity* entity)
		{
			ImGuiTreeNodeFlags flags = 0;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= (m_Selected == entity) ? ImGuiTreeNodeFlags_Selected : 0;
			bool opened = ImGui::TreeNodeEx((void*)entity, flags, entity->name.c_str());

			// Handle Draging and or Droping
			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("HIERARCHY_ENTITY_DRAG_SOURCE", entity, 64);
				ImGui::Text("%s", entity->name.c_str());
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY_DRAG_SOURCE")) {
					if (payload->DataSize != 64) LOG_SOFT_ERROR("Invalid hierarchy entity drag payload");
					Entity* draged = (Entity*)payload->Data;
					m_SceneContext->SetEntityParent(draged->ID, entity->ID);
					LOG_MESSAGE("Draged %s to %s", 5, draged->name.c_str(), entity->name.c_str());
					LOG_MESSAGE("%s is now a child of %s", 5, draged->name.c_str(), entity->name.c_str());
				}
				ImGui::EndDragDropTarget();
			}

			// Handle Short Cuts
			if (m_Selected == entity) {
				if (ImGui::IsKeyPressed(ImGuiKey_Delete)) DeleteEntity(entity);
				if (ImGui::Shortcut(ImGuiKey_N + ImGuiMod_Ctrl)) CreateChildEntity(entity);
			}

			// Handle Entity Node Clicks
			if (ImGui::IsItemClicked()) {
				m_Selected = entity;
				NewInspectorContext newContext(new EntityInspectorContext(entity));
				Core::Application::EventCallback(newContext);
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
				m_Selected = entity;
				NewInspectorContext newContext(new EntityInspectorContext(entity));
				Core::Application::EventCallback(newContext);
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