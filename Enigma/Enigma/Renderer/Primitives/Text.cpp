#include "Enigma/EnigmaPCH.h"
#include "Enigma/Renderer/Primitives/Text.h"

namespace Enigma::Renderer {
	Text::Text(ref<VertexArray> glyphBoundsVAO, ref<Texture> glyphData, const TextBoundingBox& boundingBox)
	{
		m_GlyphBoundsVAO = glyphBoundsVAO;
		m_GlyphData      = glyphData;
		m_BoundingBox    = boundingBox;
	}
}