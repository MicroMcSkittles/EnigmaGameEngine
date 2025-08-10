#include "Enigma/Renderer/Primitives/Text.h"

#include <glm/gtc/matrix_transform.hpp>
#include <map>

namespace Enigma {
	namespace Renderer {

		Text::Text(VertexArray* glyphBoundsBoxVAO, Texture* glyphData, const TextBoundingBox& boundingBox)
		{
			m_GlyphBoundsVAO = glyphBoundsBoxVAO;
			m_GlyphData      = glyphData;
			m_BoundingBox    = boundingBox;
		}

		Text::~Text()
		{
			if (m_GlyphBoundsVAO) delete m_GlyphBoundsVAO;
			if (m_GlyphData)      delete m_GlyphData;
		}
	}
}