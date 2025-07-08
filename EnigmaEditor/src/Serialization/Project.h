#pragma once
#include "Serialization/JSON.h"
#include "Scene/Scene.h"

namespace Enigma {
	namespace Editor {

		class Project {
		public:
			Project(const std::string& filename);
			~Project();

			Scene* GetActiveScene() { return m_ActiveScene; }
			void SetActiveScene(Scene* scene) { m_ActiveScene = scene; }

			void Save();

		private:
			std::string m_ProjectPath;
			JSON::DataTreeNode m_ProjectData;

			Scene* m_ActiveScene;
		};

	}
}