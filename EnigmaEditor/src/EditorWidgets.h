#pragma once
#include <Enigma/Renderer/Texture.h>

#include <glm/glm.hpp>

#include "Editor.h"
#include "Assets/TextureAsset.h"

namespace ImGui {
	 bool InputTextureAsset(const std::string& label, Enigma::Editor::TextureAsset*& output, const glm::vec2& previewSize);
}