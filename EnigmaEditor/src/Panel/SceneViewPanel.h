#pragma once
#include "Scene/SceneView.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace Editor {

		class SceneViewPanel {
		public:
			SceneViewPanel(SceneView2D* sceneView);
			~SceneViewPanel();

			void ImGui();

		private:
			glm::vec2 m_LastViewSize;
			SceneView2D* m_SceneView;
		};

	}
}