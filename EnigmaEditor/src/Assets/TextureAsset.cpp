#include "TextureAsset.h"

#include <Enigma/ImGui/EnigmaWidgets.h>
#include <imgui.h>

namespace Enigma {
	namespace Editor {

		void TextureAsset::Unload()
		{
			if (userCount != 0) return;
			delete texture;
			texture = nullptr;
		}

		void TextureAsset::Load(JSON::DataTreeNode& dataTree)
		{
			config.sWrapMode = (Renderer::TexWrapMode)(int)dataTree["sWrapMode"].value;
			config.tWrapMode = (Renderer::TexWrapMode)(int)dataTree["tWrapMode"].value;
			config.minFilter = (Renderer::TexFilterMode)(int)dataTree["MinFilter"].value;
			config.magFilter = (Renderer::TexFilterMode)(int)dataTree["MagFilter"].value;

			config.borderColor.r = dataTree["BorderColor"].At(0).value;
			config.borderColor.g = dataTree["BorderColor"].At(1).value;
			config.borderColor.b = dataTree["BorderColor"].At(2).value;

			config.flipY = dataTree["FlipY"].value;
		}
		void TextureAsset::Save(JSON::DataTreeNode& dataTree)
		{
			dataTree.format.childOrder = { "Type", "sWrapMode", "tWrapMode", "MinFilter", "MagFilter", "BorderColor", "FlipY" };
			dataTree["Type"] = (int)AssetType::Texture;

			dataTree["sWrapMode"] = (int)config.sWrapMode;
			dataTree["tWrapMode"] = (int)config.tWrapMode;
			dataTree["MinFilter"] = (int)config.minFilter;
			dataTree["MagFilter"] = (int)config.magFilter;

			dataTree["BorderColor"] = JSON::DataTreeType::Array;
			dataTree["BorderColor"].format.flags |= JSON::DataTreeFormatFlag::Collapse;
			dataTree["BorderColor"].elements.push_back(config.borderColor.r);
			dataTree["BorderColor"].elements.push_back(config.borderColor.g);
			dataTree["BorderColor"].elements.push_back(config.borderColor.b);

			dataTree["FlipY"] = config.flipY;
		}

		Renderer::Texture*& TextureAsset::GetAssetIcon()
		{
			if (texture == nullptr) {
				if(!loader.IsLoading()) loader.Load(texture, path, config);
			}
			return texture;
		}

		InspectorContext* TextureAsset::GetInspectorContext()
		{
			return new TextureAssetInspectorContext(this);
		}

		TextureAssetInspectorContext::TextureAssetInspectorContext(TextureAsset* asset) : m_Asset(asset)
		{
			m_Asset->userCount += 1;
		}

		TextureAssetInspectorContext::~TextureAssetInspectorContext()
		{
			m_Asset->userCount -= 1;
			m_Asset->Unload();
		}

		void TextureAssetInspectorContext::ImGui()
		{
			ImGui::BeginChild("TEXTURE_ASSET_INSPECTOR_CONTEXT_IMAGE_VIEW", { 0,350 });
			if (m_Asset->texture) ImGui::Image(m_Asset->texture);
			else if (!m_Asset->loader.IsLoading()) m_Asset->loader.Load(m_Asset->texture, m_Asset->path, m_Asset->config);
			ImGui::EndChild();
			ImGui::SeparatorText(m_Asset->name.c_str());

			int sWrapMode = (int)m_Asset->config.sWrapMode - 1;
			ImGui::Combo("S Wrap Mode", &sWrapMode, "Repeat\0Mirrored Repeat\0Clamp To Edge\0Clamp To Border");
			m_Asset->config.sWrapMode = (Renderer::TexWrapMode)(sWrapMode + 1);

			int tWrapMode = (int)m_Asset->config.tWrapMode - 1;
			ImGui::Combo("T Wrap Mode", &tWrapMode, "Repeat\0Mirrored Repeat\0Clamp To Edge\0Clamp To Border");
			m_Asset->config.tWrapMode = (Renderer::TexWrapMode)(tWrapMode + 1);

			int minFilter = (int)m_Asset->config.minFilter - 1;
			ImGui::Combo("Min Filter", &minFilter, "Nearest\0Linear");
			m_Asset->config.minFilter = (Renderer::TexFilterMode)(minFilter + 1);

			int magFilter = (int)m_Asset->config.magFilter - 1;
			ImGui::Combo("Mag Filter", &magFilter, "Nearest\0Linear");
			m_Asset->config.magFilter = (Renderer::TexFilterMode)(magFilter + 1);

			ImGui::ColorEdit3("Border Color", &m_Asset->config.borderColor.r);

			ImGui::Checkbox("Flip Y", &m_Asset->config.flipY);

			if (ImGui::Button("Reload")) {

			}
		}

	}
}