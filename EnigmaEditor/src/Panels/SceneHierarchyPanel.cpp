#include "Panels/SceneHierarchyPanel.h"
#include "EditorImGui.h"
#include "EditorEvents.h"
#include "UndoRedoAction.h"

#include <Enigma/Core/Process/Application.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Enigma::Editor {

	SceneHierachyPanel::SceneHierachyPanel()
	{
		m_OpenEntitySettings = false;
		m_RenameEntity = false;
		m_FromCreate = false;
	}

	void SceneHierachyPanel::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);

		handler.Dispatch<Event::EntitySelected>([&](Event::EntitySelected& e) {
			m_Selected = e.GetEntity();
			return false;
		});
		handler.Dispatch<Event::SceneChange>([&](Event::SceneChange& e) {
			m_Selected = { };
			SetContext(e.GetScene());
			return false;
		});
	}

	void SceneHierachyPanel::ShowGui()
	{
		ImGui::Begin("Scene Hierachy");

		if (m_SceneContext == nullptr) {
			ImGui::TextWrapped("No scene selected");
			ImGui::End();
			return;
		}

		ImGui::BeginChild("SceneHierachyView", ImVec2(0, 0), ImGuiChildFlags_ResizeY);

		// Show root entities
		for (Entity entity : m_SceneContext->GetRoot().children.GetData()) {
			EntityMetaData& metaData = entity.GetComponent<EntityMetaData>();
			EntityNodeGui(entity, metaData);
		}

		ImGui::EndChild();

		// Root Entity settings menu
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) ImGui::OpenPopup("RootEntitySettings");
		RootEntitySettings();

		// Entity settings menu
		if (m_OpenEntitySettings) {
			ImGui::OpenPopup("EntitySettings");
			m_OpenEntitySettings = false;
		}
		EntitySettings();
		

		EntityDragDropTarget({});

		ImGui::End();
	}

	void SceneHierachyPanel::EntityNodeGui(Entity entity, EntityMetaData& metaData)
	{
		// Push entity id to imgui
		ImGui::PushID(entity.GetID());

		// Configure tree node
		ImGuiTreeNodeFlags flags = 0;
		flags |= ImGuiTreeNodeFlags_AllowOverlap;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= (metaData.children.Size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0;
		flags |= (m_Selected == entity) ? ImGuiTreeNodeFlags_Selected : 0;

		// Open tree node
		if (m_SceneContext->IsChild(m_Selected, entity)) ImGui::SetNextItemOpen(true);
		std::string nodeName = (entity == m_EntityToRename) ? "" : metaData.name;
		bool open = ImGui::TreeNodeEx(metaData.name.c_str(), flags, nodeName.c_str());

		// Rename gui
		if (entity == m_EntityToRename) {
			ImGui::SameLine();
			EntityRenameGui();
		}

		// Handle DragDrop
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("Entity", &entity, sizeof(Entity));
		
			ImGui::Text("%s", metaData.name.c_str());
			ImGui::EndDragDropSource();
		}
		EntityDragDropTarget(entity);

		// Handle clicks
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			m_Selected = entity;
			//m_SelectionCallback(m_Selected);
			Event::EntitySelected e(m_Selected);
			Core::Application::EventCallback(e);
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			m_EntitySettingsContext = entity;
			m_OpenEntitySettings = true;
		}

		// Make sure the tree node is open
		if (!open) goto CloseGui;

		// Show child entities
		for (Entity child : metaData.children.GetData()) {
			EntityNodeGui(child, child.GetComponent<EntityMetaData>());
		}

		ImGui::TreePop();

		// Close entity node
		CloseGui:
		ImGui::PopID();
	}

	void SceneHierachyPanel::EntityRenameGui()
	{
		EntityMetaData& metaData = m_EntityToRename.GetComponent<EntityMetaData>();
	
		if (m_OldName.empty()) m_OldName = metaData.name;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		
		// Set Keyboard focus on the text box
		if (m_RenameEntity) {
			ImGui::SetKeyboardFocusHere();
			m_RenameEntity = false;
		}

		// Show text box
		ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
		std::string buffer = metaData.name;
		Entity entity = m_EntityToRename;
		if (ImGui::InputText("##EntityRenameBox", &buffer, inputFlags)) {
			metaData.name = buffer;
			m_EntityToRename = { };
		}
		if (ImGui::IsItemDeactivated()) {		
			metaData.name = buffer;
			m_EntityToRename = { };
		}
		if (m_EntityToRename == Entity() && !m_FromCreate) {
			RenameEntityAction(entity, metaData.name, m_OldName);
			m_OldName = "";
		}

		ImGui::PopStyleVar();
	}
	void SceneHierachyPanel::EntitySettings()
	{
		if (!ImGui::BeginPopup("EntitySettings")) {
			m_EntitySettingsContext = {};
			return;
		}

		if (ImGui::MenuItem("Rename")) {
			m_EntityToRename = m_EntitySettingsContext;
			m_Selected = m_EntitySettingsContext;
			m_RenameEntity = true;
			m_FromCreate = false;
		}

		if (ImGui::MenuItem("Create Child")) {
			m_EntityToRename = m_SceneContext->CreateEntity(m_EntitySettingsContext, "New Entity");
			m_Selected = m_EntityToRename;
			m_RenameEntity = true;
			m_FromCreate = true;
			//m_SelectionCallback(m_Selected);
			Event::EntitySelected e(m_Selected);
			Core::Application::EventCallback(e);

			CreateEntityAction(m_SceneContext, m_Selected);
		}

		if (ImGui::MenuItem("Remove")) {
			RemoveEntityAction(m_SceneContext, m_Selected);
			m_SceneContext->RemoveEntity(m_EntitySettingsContext);
		}

		ImGui::EndPopup();
	}
	void SceneHierachyPanel::RootEntitySettings()
	{
		if (!ImGui::BeginPopup("RootEntitySettings")) return;

		if (ImGui::MenuItem("Create")) {
			m_EntityToRename = m_SceneContext->CreateEntity("New Entity");
			m_Selected = m_EntityToRename;
			m_RenameEntity = true;
			m_FromCreate = true;
			
			Event::EntitySelected e(m_Selected);
			Core::Application::EventCallback(e);

			CreateEntityAction(m_SceneContext, m_Selected);
		}

		ImGui::EndPopup();
	}
	void SceneHierachyPanel::EntityDrag(Entity entity, EntityMetaData& metaData)
	{
		//if (!(ImGui::IsItemActive() && !ImGui::IsItemHovered())) return;

		//f32 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y;
		//if (fabsf(mouseDelta) < 10.f) return;
		//i32 delta = (mouseDelta < 0.0f ? -1 : 1);
		if (m_Selected != entity) return;
		
		i32 delta = 0;
		if (ImGui::IsKeyChordPressed(ImGuiKey_UpArrow | ImGuiKey_ModShift)) delta = -1;
		else if (ImGui::IsKeyChordPressed(ImGuiKey_DownArrow | ImGuiKey_ModShift)) delta = 1;
		else return;

		ImGui::PushItemFlag(ImGuiItemFlags_AllowDuplicateId, true);

		// Find what set the entity is in
		Core::SparseSet<Entity>* entities = nullptr;
		if (metaData.parent.Valid()) entities = &metaData.parent.GetComponent<EntityMetaData>().children;
		else entities = &m_SceneContext->GetRoot().children;

		u64 index = entities->GetDenseIndex(entity.GetID());

		// Find new index
		index += delta;
		if (delta < 0 && index >= entities->Size()) index = 0;
		else if (delta > 0 && index >= entities->Size()) index = entities->Size() - 1;

		Entity next = entities->GetData().at(index);

		entities->Swap(entity.GetID(), next.GetID());

		ImGui::ResetMouseDragDelta();

		ImGui::PopItemFlag();
	}
	void SceneHierachyPanel::EntityDragDropTarget(Entity entity)
	{
		if (!ImGui::BeginDragDropTarget()) return;
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();

		// Check payload
		if (payload == nullptr) goto CloseGui;
		if (std::string(payload->DataType) != "Entity") goto CloseGui;

		// Makes sure the user can't nest an entity in itself
		Entity child = *static_cast<Entity*>(payload->Data);
		if (m_SceneContext->IsChild(entity, child)) goto CloseGui;

		// Accept the payload
		ImGui::AcceptDragDropPayload("Entity");
		if (!payload->IsDelivery()) goto CloseGui;

		// Update other entity parent
		EntityMetaData& otherMetaData = child.GetComponent<EntityMetaData>();
		ChangeParentAction(m_SceneContext, child, entity);
		m_SceneContext->ChangeParent(child, entity);

		// Close dragdrop target
		CloseGui:
		ImGui::EndDragDropTarget();
	}

}