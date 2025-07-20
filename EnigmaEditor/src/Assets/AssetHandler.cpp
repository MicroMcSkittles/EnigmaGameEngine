#include "Assets/AssetHandler.h"
#include "Assets/TextureAsset.h"
#include "EditorIcons.h"

#include <filesystem>

namespace Enigma {
	namespace Editor {
		AssetHandler::AssetHandler(const std::string& assetPath)
		{
			m_AssetsPath = assetPath;
		}
		AssetHandler::AssetHandler(const std::string& assetPath, JSON::DataTreeNode& dataTree)
		{
			m_AssetsPath = assetPath;

			for (auto& [path, assetData] : dataTree.children) {
				AssetType type = (AssetType)(int)assetData["Type"].value;

				Asset* asset = nullptr;

				switch (type)
				{
				case AssetType::Texture: asset = new TextureAsset(); break;
				default: continue;
				}

				asset->name = ((std::filesystem::path)path).filename().string();
				asset->path = path;
				asset->type = type;
				asset->Load(assetData);

				m_Assets.insert({ path, asset });
			}
		}
		AssetHandler::~AssetHandler()
		{
			for (auto& [path, asset] : m_Assets) {
				delete asset;
			}
		}

		void AssetHandler::Save(JSON::DataTreeNode& dataTree)
		{
			for (auto& [path, asset] : m_Assets) {
				JSON::DataTreeNode assetData = JSON::DataTreeType::Object;
				asset->Save(assetData);
				dataTree.children.insert({ path, assetData });
				dataTree.format.childOrder.push_back(path);
			}
		}

		Asset* AssetHandler::GetAsset(const std::string& path)
		{
			if (!m_Assets.count(path)) {
				m_Assets.insert({ path, CreateAsset(path) });
			}
			return m_Assets[path];
		}

		Asset* AssetHandler::CreateAsset(const std::string& path)
		{
			AssetType type = Asset::AssetTypeMap[std::filesystem::path(path).extension().string()];
			Asset* asset;
			switch (type)
			{
			case AssetType::Texture: asset = new TextureAsset(); break;
			default: return nullptr;
			}

			asset->name = ((std::filesystem::path)path).filename().string();
			asset->path = path;
			asset->type = type;

			return asset;
		}

		
		Renderer::Texture*& Asset::GetAssetIcon()
		{
			return Icons::File;
		}

		InspectorContext* Asset::GetInspectorContext()
		{
			return new NullInspectorContext();
		}

	}
}