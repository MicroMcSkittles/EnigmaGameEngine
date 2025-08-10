#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "Enigma/Renderer/VertexArray.h"
#include "Enigma/Renderer/Texture.h"

namespace Enigma {
	namespace Renderer {

		enum TextPointType {
			TextPointType_None           = 0,
			TextPointType_OnCurve        = BIT(0),
			TextPointType_Implied        = BIT(1),
			TextPointType_Control        = BIT(2),
			TextPointType_StartOfContour = BIT(3)
		};
		struct TextPoint {
			glm::vec2 position;
			uint8_t type;
		};

		struct TextBoundingBox {
			glm::vec2 size;
			glm::vec2 position;
			glm::vec2 min;
			glm::vec2 max;
		};
		struct GlyphContour {
			int start;
			int end;
		};
		struct Glyph {
			std::vector<TextPoint> points;
			std::vector<GlyphContour> contours;

			TextBoundingBox boundingBox;
			int horizontalAdvance;
		};

		class Text {
		public:
			Text(VertexArray* glyphBoundsVAO, Texture* glyphData, const TextBoundingBox& boundingBox);
			~Text();

			VertexArray*     GetGlyphBoundsVAO() { return m_GlyphBoundsVAO; }
			Texture*         GetGlyphData()      { return m_GlyphData;      }
			TextBoundingBox& GetBoundingBox()    { return m_BoundingBox;    }

		private:
			VertexArray*    m_GlyphBoundsVAO;
			Texture*        m_GlyphData;
			TextBoundingBox m_BoundingBox;
		};

	}
}