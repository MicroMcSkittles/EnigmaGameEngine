#include "Enigma/EnigmaPCH.h"
#include "EnigmaWidgets.h"

#include <imgui.h>

namespace ImGui {

	void Image(Enigma::ref<Enigma::Renderer::Texture> texture, const glm::vec2& scale, const glm::vec2& uv0, const glm::vec2& uv1) {
		glm::vec2 dim = {
			texture->GetWidth(),
			texture->GetHeight()
		};

		ImVec2 size;
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (scale.x == 0 && scale.y == 0) {
			size = viewportSize;

			size.y = (dim.y * size.x) / dim.x;

			if (size.y > viewportSize.y) {
				size.y = viewportSize.y;
				size.x = (dim.x * size.y) / dim.y;
			}
		}
		else if (scale.x != 0 && scale.y == 0) {
			size.x = scale.x;
			size.y = (dim.y * size.x) / dim.x;
		}
		else if (scale.x == 0 && scale.y != 0) {
			size.y = scale.y;
			size.x = (dim.x * size.y) / dim.y;
		}
		else {
			size.x = scale.x;
			size.y = scale.y;
		}

		// Stupid ahh casts
		void* id = reinterpret_cast<void*>(static_cast<u64>(*static_cast<u32*>(texture->GetNativeTexture())));

		ImGui::Image(id, size, ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y));
	}

}