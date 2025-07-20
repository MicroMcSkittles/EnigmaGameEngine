#pragma once
#include "Serialization/JSON.h"
#include "Scene/Scene.h"
#include "Assets/AssetHandler.h"

namespace Enigma {
	namespace Editor {

		class Project {
		public:
			Project(const std::string& filename);
			~Project();

			// returns the path to the project json file
			std::string GetPath() { return m_ProjectPath; }
			// returns a path to the projects root directory
			std::string GetRootPath();
			// returns a path to the projects assets directory
			std::string GetAssetsPath() { return m_RootPath + m_AssetsPath; }

			AssetHandler* GetAssetHandler() { return m_AssetHandler; }

			Scene* GetActiveScene() { return m_ActiveScene; }
			void SetActiveScene(Scene* scene);
			void LoadScene();

			void Save();

		private:
			std::string m_ProjectPath;
			std::string m_RootPath;
			std::string m_AssetsPath; // Relative to the root path
			JSON::DataTreeNode m_ProjectData;

			Scene* m_ActiveScene;
			AssetHandler* m_AssetHandler;
		};

	}
}