#pragma once
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

namespace Enigma::Editor {
	
	class SceneHierachyPanel {
	public:
		SceneHierachyPanel();
		void SetContext(ref<Scene> scene) { m_SceneContext = scene; }
		void SetSelectionCallback(std::function<void(Entity)> callback) { m_SelectionCallback = callback; }

		void ShowGui();

	private:
		void EntityNodeGui(Entity entity, EntityMetaData& metaData);
		void EntityRenameGui();
		void EntitySettings();
		void RootEntitySettings();
		void EntityDrag(Entity entity, EntityMetaData& metaData);
		void EntityDragDropTarget(Entity entity);

	private:
		ref<Scene> m_SceneContext;
		std::function<void(Entity)> m_SelectionCallback;
		Entity m_Selected;

		bool m_OpenEntitySettings; // Used to open the entity settings menu outside the imgui tree
		Entity m_EntitySettingsContext;

		bool m_RenameEntity; // Used to set keyboard focus on the entity being renamed
		Entity m_EntityToRename;
	};

}