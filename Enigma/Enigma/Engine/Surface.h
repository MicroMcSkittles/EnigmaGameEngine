#pragma once
#include "Enigma/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Enigma::Engine {

	struct Surface {
		glm::vec2 position = { 0, 0 }; // the position of the surface relative to the windows origin
		glm::vec2 scale = { 0, 0 }; // the size of the surface
		ref<Renderer::Texture> frame; // the engine will render to this texture, if it's nullptr then the engine renders to the window
		
		void Resize(int width, int height)
		{
			if (scale.x == width && scale.y == height) return;
			scale = { width, height };
			if(frame) frame->Resize(width, height);
		}
	};

}