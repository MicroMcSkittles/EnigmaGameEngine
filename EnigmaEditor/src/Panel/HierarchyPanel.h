#pragma once
#include "Scene/Scene.h"

#include <functional>

namespace Enigma {
	namespace Editor {

		class HierarchyPanel {
		public:
			HierarchyPanel(std::function<void(Entity*)> selectCallback)
				: m_SceneContext(nullptr), m_Selected(nullptr), m_IsMenuOpen(false),
				m_SelectCallback(selectCallback) { }

			void SetContext(Scene* scene) { m_SceneContext = scene; }

			void ImGui();

		private:
			void DeleteEntity(Entity* entity);
			void CreateChildEntity(Entity* parent);
			void CreateEntity();

			void EntityRightClickMenu(Entity* entity);
			void RightClickMenu();

			void EntityNode(Entity* entity);

		private:
			std::function<void(Entity*)> m_SelectCallback;
			Entity* m_Selected;
			Scene* m_SceneContext;
			bool m_IsMenuOpen;
		};

	}
}