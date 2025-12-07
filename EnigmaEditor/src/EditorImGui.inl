


namespace Enigma::Editor {
	template <typename... Comps>
	inline bool EditorGui::InputEntity(const std::string& lable, Entity& entity, const ref<Scene>& scene, f32 columnWidth) {

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		std::vector<Entity> entities;
		i32 selection = -1;
		u32 entityCount = 0;

		// Find all entities with Comps components
		Engine::ECS::View<Comps...> entityView(scene->GetECS());
		entityView.ForEach([&](Engine::ECS::EntityID id) {
			entities.push_back({ id, scene.get() });
			if (id == entity.GetID()) selection = entityCount;
			entityCount += 1;
		});

		// Handle drop down menu
		bool edited = ImGui::Combo(
			("##InputEntityComboBox_" + lable).c_str(), 
			&selection,
			[](void* user_data, int idx) {
				return static_cast<std::vector<Entity>*>(user_data)->at(idx).GetComponent<EntityMetaData>().name.c_str();
			},
			static_cast<void*>(&entities), 
			entities.size()
		);
		if (edited) entity = entities[selection];

		// Handle draged entities
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();

			// Check payload
			if (payload == nullptr) goto CloseDragTarget;
			if (std::string(payload->DataType) != EntityDragSourceName) goto CloseDragTarget;
			
			Entity other = *static_cast<Entity*>(payload->Data);
			if (!other.HasComponents<Comps...>()) goto CloseDragTarget;

			// Accept the payload
			ImGui::AcceptDragDropPayload(EntityDragSourceName.c_str());
			if (!payload->IsDelivery()) goto CloseDragTarget;

			entity = other;
			edited = true;

			CloseDragTarget:
			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleVar();

		ImGui::EndColumns();
		ImGui::PopID();

		return edited;
	}
}