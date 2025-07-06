#pragma once
#include "Panel/InspectorPanel.h"
#include "Scene/Component.h"

#include <Enigma/Core/IdHandler.h>
#include <string>

namespace Enigma {
	namespace Editor {

		class Scene;

		// Used to make editing the entities easier
		struct Entity {
			std::string name;

			Core::ID ID; // ID for a Scene entity 

			Core::ID entityID; // ID for the ECS entity
			Core::ID parentID; // ID for a Scene entity
			std::vector<Core::ID> childrenIDs; // IDs for Scene entities

			Scene* scene;

			// Returns the entity's name in the format
			// (Parent Entity Path Name)/name
			std::string GetPathName();
		};

		class EntityInspectorContext : public InspectorContext {
		public:
			EntityInspectorContext(Entity* entity);
			~EntityInspectorContext();

			virtual void ImGui() override;

		private:
			void CreateComponentImGui();

		private:
			std::vector<ComponentContext*> m_Components;
			Entity* m_Entity;
			bool m_ComponentPopup;
		};

	}
}