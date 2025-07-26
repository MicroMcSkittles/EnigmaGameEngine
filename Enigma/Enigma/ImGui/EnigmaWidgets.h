#pragma once
#include "Enigma/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace ImGui {

	// When scale is { 0,0 } the image gets resized automaticaly
	// If either x or y is not 0 then the other axis is resized to keep the same proportions
	// If both x and y are not 0 then the texture gets scaled to that exact size
	void Image(Enigma::Renderer::Texture* texture, const glm::vec2& scale = { 0,0 }, const glm::vec2& uv0 = { 0,1 }, const glm::vec2& uv1 = { 1,0 });

}