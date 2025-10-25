#include "Enigma/Renderer/Primitives/Text.h"

#include <glm/gtc/matrix_transform.hpp>
#include <map>

namespace Enigma {
	namespace Renderer {

		Text::Text(ref<VertexArray> glyphBoundsVAO, ref<Texture> glyphData, const TextBoundingBox& boundingBox)
		{
			m_GlyphBoundsVAO = glyphBoundsVAO;
			m_GlyphData      = glyphData;
			m_BoundingBox    = boundingBox;
		}
	}
}