#pragma once
#include "Assets/AssetHandler.h"

namespace Enigma {
	namespace Editor {

		struct TextureAsset : public Asset {
			Renderer::Texture* texture;
			ImageConfig config;
			ImageLoader loader;

			virtual void Unload() override;

			virtual void Load(JSON::DataTreeNode& dataTree) override;
			virtual void Save(JSON::DataTreeNode& dataTree) override;

			virtual Renderer::Texture*& GetAssetIcon() override;
			virtual InspectorContext* GetInspectorContext() override;
		};

		class TextureAssetInspectorContext : public InspectorContext {
		public:
			TextureAssetInspectorContext(TextureAsset* asset);
			~TextureAssetInspectorContext();
			virtual void ImGui() override;

		private:
			TextureAsset* m_Asset;
		};

	}
}