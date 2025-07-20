#include "Serialization/Project.h"
#include "Utils.h"

#include <filesystem>

namespace Enigma {
	namespace Editor {

		Project::Project(const std::string& filename)
		{
			m_ProjectPath = filename;
			JSON::JSON::LoadFile(m_ProjectPath, &m_ProjectData);

			m_RootPath = m_ProjectData["RootPath"].value;
			m_AssetsPath = m_ProjectData["AssetsPath"].value;

			if (m_ProjectData["ActiveScene"].value.IsNull()) {
				m_ActiveScene = nullptr;
				return;
			}

			m_AssetHandler = new AssetHandler(m_AssetsPath, m_ProjectData["Assets"]);

			
		}

		Project::~Project()
		{
			Save();
			if (m_ActiveScene) delete m_ActiveScene;
			if (m_AssetHandler) delete m_AssetHandler;
		}

		std::string Project::GetRootPath()
		{
			std::filesystem::path root = m_ProjectPath;
			root = root.parent_path();
			root = root.string() + "\\" + m_RootPath;
			return CondensePath(root).string();
		}

		void Project::SetActiveScene(Scene* scene)
		{
			if (m_ActiveScene != nullptr) {
				m_ActiveScene->Save();
				delete m_ActiveScene;
			}
			m_ActiveScene = scene;
		}

		void Project::LoadScene()
		{
			std::string scenePath = m_ProjectData["ActiveScene"].value;
			m_ActiveScene = new Scene(GetRootPath() + scenePath);
		}

		void Project::Save()
		{
			m_ActiveScene->Save();

			m_ProjectData.format.childOrder = { "RootPath", "AssetsPath", "ActiveScene", "Assets"};
			m_ProjectData["RootPath"] = m_RootPath;
			m_ProjectData["AssetsPath"] = m_AssetsPath;

			if (m_ActiveScene == nullptr) m_ProjectData["ActiveScene"] = JSON::DataTreeType::Null;
			else {
				std::string activeScenePath = m_ActiveScene->GetPath();
				activeScenePath = std::string(activeScenePath.begin() + GetRootPath().size(), activeScenePath.end());
				m_ProjectData["ActiveScene"] = activeScenePath;
			}

			m_ProjectData["Assets"] = JSON::DataTreeType::Object;
			m_AssetHandler->Save(m_ProjectData["Assets"]);

			JSON::JSON::Serialize(m_ProjectPath, &m_ProjectData);
		}

	}
}