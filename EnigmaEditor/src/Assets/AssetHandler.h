#pragma once
#include <string>
#include <map>

#include <Enigma/Renderer/Texture.h>
#include "Serialization/JSON.h"
#include "Serialization/Image.h"
#include "Panel/InspectorPanel.h"

namespace Enigma {
	namespace Editor {

		enum class AssetType {
			None = 0,
			Texture
		};

		
		struct Asset {
			AssetType type;

			std::string name;
			std::string path;

			int userCount = 0;

			virtual void Unload() { }

			virtual void Load(JSON::DataTreeNode& dataTree) = 0;
			virtual void Save(JSON::DataTreeNode& dataTree) = 0;

			virtual Renderer::Texture*& GetAssetIcon();
			virtual InspectorContext* GetInspectorContext();

			inline static std::map<std::string, AssetType> AssetTypeMap = {
				{ ".png", AssetType::Texture },
				{ ".jpg", AssetType::Texture },
				{ ".jpeg", AssetType::Texture }
			};
		};

		class AssetHandler {
		public:
			AssetHandler(const std::string& assetPath);
			AssetHandler(const std::string& assetPath, JSON::DataTreeNode& dataTree);
			~AssetHandler();

			void Save(JSON::DataTreeNode& dataTree);

			Asset* GetAsset(const std::string& path);

			Asset* CreateAsset(const std::string& path);

		private:
			std::string m_AssetsPath;
			std::map<std::string, Asset*> m_Assets;
		};

	}
}