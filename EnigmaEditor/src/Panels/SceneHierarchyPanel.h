#pragma once
#include <Enigma/Core/Event/Event.h>
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

namespace Enigma::Editor {
	
	class SceneHierachyPanel {
	public:
		SceneHierachyPanel();
		void SetContext(ref<Scene> scene) { m_SceneContext = scene; }

		void OnEvent(Core::Event& e);
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
		Entity m_Selected;

		bool m_OpenEntitySettings; // Used to open the entity settings menu outside the imgui tree
		Entity m_EntitySettingsContext;

		bool m_RenameEntity; // Used to set keyboard focus on the entity being renamed
		bool m_FromCreate;
		Entity m_EntityToRename;
		std::string m_OldName;
	};

}