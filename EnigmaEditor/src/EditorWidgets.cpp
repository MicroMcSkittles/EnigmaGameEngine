#include "EditorWidgets.h"
#include "EditorIcons.h"
#include "Panel/ContentBrowserPanel.h"

#include <Enigma/Core/Core.h>

#include <Enigma/ImGui/EnigmaWidgets.h>
#include <imgui.h>

namespace ImGui {

    bool InputTextureAsset(const std::string& label, Enigma::Editor::TextureAsset*& output, const glm::vec2& previewSize) {

        using namespace Enigma::Editor;

        bool pushedID = output != nullptr;
        if (pushedID) ImGui::PushID((void*)output);

        ImGui::BeginChild("INPUT_TEXTURE_ASSET", {0,0}, ImGuiChildFlags_AutoResizeY);

        // Find preview image, if it can't find a texture then show the NoTexture icon
        Enigma::Renderer::Texture* previewImage = nullptr;
        std::string previewImageName = "No Name";
        if (output == nullptr) {
            previewImage = Icons::NoTexture;
        }
        else if (output->texture == nullptr) {
            previewImage = Icons::NoTexture;
            if (!output->loader.IsLoading()) output->loader.Load(output->texture, output->path, {});
            previewImageName = output->name;
        }
        else {
            previewImage = output->texture;
            previewImageName = output->name;
        }

        // Show preview
        ImGui::Image(previewImage, previewSize);

        ImGui::SameLine();

        ImGui::BeginChild("INPUT_TEXTURE_ASSET_INFO");
        ImGui::SeparatorText(label.c_str());
        ImGui::Text(previewImageName.c_str());

        ImGui::EndChild();
        ImGui::EndChild();

        // Handle draged textures
        if (!ImGui::BeginDragDropTarget()) {
            if (pushedID) ImGui::PopID();
            return false;
        }
        
        bool changedTexture = false;

        // Get payload
        const ImGuiPayload* payload = ImGui::GetDragDropPayload();
        if (payload == nullptr) goto END_FUNC;
        if (!payload->IsDataType("CONTENT_BROWSER_FILE_DRAG_SOURCE")) goto END_FUNC;
        if (payload->DataSize != sizeof(File)) goto END_FUNC;

        File* file = (File*)payload->Data;
        if (file->type != FileType::Asset) goto END_FUNC;

        if (ImGui::AcceptDragDropPayload("CONTENT_BROWSER_FILE_DRAG_SOURCE") == nullptr) goto END_FUNC;

        output = (TextureAsset*)Editor::GetActiveProject()->GetAssetHandler()->GetAsset(file->path.string());
        
        changedTexture = true;

        // Wrap up every thing
        END_FUNC:
        ImGui::EndDragDropTarget();
        if (pushedID) ImGui::PopID();
        return changedTexture;
    }

}
