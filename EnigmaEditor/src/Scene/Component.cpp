#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include "EditorWidgets.h"
#include "Editor.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Enigma {
	namespace Editor {

		void EditorTag::Save(JSON::DataTreeNode& dataTree)
		{
			dataTree = JSON::DataTreeType::Object;
			dataTree.format.childOrder = { "Type", "Tag" };
			dataTree["Type"] = "Tag";

			Engine::ECS::Tag& component = GetComponent();
			dataTree["Tag"] = component.tag;
		}
		void EditorTag::Load(JSON::DataTreeNode& dataTree)
		{
			Engine::ECS::Tag& component = GetComponent();
			component.tag = dataTree["Tag"].value;
		}
		void EditorTag::InspectorImGuiImpl(Engine::ECS::Tag& component)
		{
			ImGui::InputText("Tag", &component.tag);
		}

		void EditorTransform::Save(JSON::DataTreeNode& dataTree)
		{
			dataTree = JSON::DataTreeType::Object;
			dataTree.format.childOrder = { "Type", "Relative", "Position", "Rotation", "Scale" };
			dataTree["Type"] = "Transform";

			Engine::ECS::Transform& component = GetComponent();

			dataTree["Relative"] = component.relative;

			dataTree["Position"] = JSON::DataTreeType::Array;
			dataTree["Position"].format.flags |= JSON::Collapse;
			dataTree["Position"].elements.push_back(component.position.x);
			dataTree["Position"].elements.push_back(component.position.y);
			dataTree["Position"].elements.push_back(component.position.z);

			dataTree["Rotation"] = JSON::DataTreeType::Array;
			dataTree["Rotation"].format.flags |= JSON::Collapse;
			dataTree["Rotation"].elements.push_back(component.rotation.x);
			dataTree["Rotation"].elements.push_back(component.rotation.y);
			dataTree["Rotation"].elements.push_back(component.rotation.z);

			dataTree["Scale"] = JSON::DataTreeType::Array;
			dataTree["Scale"].format.flags |= JSON::Collapse;
			dataTree["Scale"].elements.push_back(component.scale.x);
			dataTree["Scale"].elements.push_back(component.scale.y);
			dataTree["Scale"].elements.push_back(component.scale.z);
		}
		void EditorTransform::Load(JSON::DataTreeNode& dataTree)
		{
			Engine::ECS::Transform& component = GetComponent();
			component.relative = dataTree["Relative"].value;

			component.position.x = dataTree["Position"].elements[0].value;
			component.position.y = dataTree["Position"].elements[1].value;
			component.position.z = dataTree["Position"].elements[2].value;

			component.rotation.x = dataTree["Rotation"].elements[0].value;
			component.rotation.y = dataTree["Rotation"].elements[1].value;
			component.rotation.z = dataTree["Rotation"].elements[2].value;

			component.scale.x = dataTree["Scale"].elements[0].value;
			component.scale.y = dataTree["Scale"].elements[1].value;
			component.scale.z = dataTree["Scale"].elements[2].value;
		}
		void EditorTransform::InspectorImGuiImpl(Engine::ECS::Transform& component)
		{
			ImGui::Checkbox("Relative", &component.relative);
			ImGui::DragFloat3("Position", &component.position.x);
			ImGui::DragFloat3("Rotation", &component.rotation.x);
			ImGui::DragFloat3("Scale", &component.scale.x);
		}

		EditorRender2D::~EditorRender2D()
		{
			if (m_TextureAsset == nullptr) return;
			m_TextureAsset->userCount -= 1;
			m_TextureAsset->Unload();
		}

		void EditorRender2D::Save(JSON::DataTreeNode& dataTree)
		{
			dataTree.value.type = JSON::DataTreeType::Object;
			dataTree.format.childOrder = { "Type", "Depth", "Tint", "Texture" };
			dataTree["Type"] = "Render2D";

			Engine::ECS::Render2D& component = GetComponent();

			dataTree["Depth"] = component.depth;

			dataTree["Tint"] = JSON::DataTreeType::Array;
			dataTree["Tint"].format.flags |= JSON::Collapse;
			dataTree["Tint"].elements.push_back(component.tint.x);
			dataTree["Tint"].elements.push_back(component.tint.y);
			dataTree["Tint"].elements.push_back(component.tint.z);
			dataTree["Tint"].elements.push_back(component.tint.w);

			if (m_TextureAsset == nullptr) dataTree["Texture"] = JSON::DataTreeType::Null;
			else dataTree["Texture"] = m_TextureAsset->path;
		}
		void EditorRender2D::Load(JSON::DataTreeNode& dataTree)
		{
			Engine::ECS::Render2D& component = GetComponent();

			component.depth = dataTree["Depth"].value;

			component.tint.x = dataTree["Tint"].elements[0].value;
			component.tint.y = dataTree["Tint"].elements[1].value;
			component.tint.z = dataTree["Tint"].elements[2].value;
			component.tint.w = dataTree["Tint"].elements[3].value;

			if (dataTree["Texture"].value.IsNull()) component.texture = nullptr;
			else {
				Asset* asset = Editor::GetActiveProject()->GetAssetHandler()->GetAsset(dataTree["Texture"].value);
				if (asset->type != AssetType::Texture) {
					LOG_WARNING("Failed to load texture for entity ( %s ) invailed asset path ( %s )", m_Parent->name.c_str(), asset->path.c_str());
					component.texture = nullptr;
				}
				else {
					m_TextureAsset = (TextureAsset*)asset;
					m_TextureAsset->userCount += 1;
				}
			}
		}
		void EditorRender2D::InspectorImGuiImpl(Engine::ECS::Render2D& component)
		{
			ImGui::InputInt("Depth", &component.depth);
			ImGui::ColorEdit4("Tint", &component.tint.r);

			if (ImGui::InputTextureAsset("Texture", m_TextureAsset, { 0, 128 })) {
				if (m_TextureAsset->texture != nullptr) component.texture = m_TextureAsset->texture;
				LOG_MESSAGE("New texture ( %s ) asset for %s", 5, m_TextureAsset->name.c_str(), m_Parent->GetPathName().c_str());
			}
		}
	}
}