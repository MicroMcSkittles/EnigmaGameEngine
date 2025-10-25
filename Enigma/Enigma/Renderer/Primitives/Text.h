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
			u8 type;
		};

		struct TextBoundingBox {
			glm::vec2 size;
			glm::vec2 position;
			glm::vec2 min;
			glm::vec2 max;
		};
		struct GlyphContour {
			i32 start;
			i32 end;
		};
		struct Glyph {
			std::vector<TextPoint> points;
			std::vector<GlyphContour> contours;

			TextBoundingBox boundingBox;
			i32 horizontalAdvance;
		};

		class Text {
		public:
			Text(ref<VertexArray> glyphBoundsVAO, ref<Texture> glyphData, const TextBoundingBox& boundingBox);
			~Text() { }

			ref<VertexArray> GetGlyphBoundsVAO() { return m_GlyphBoundsVAO; }
			ref<Texture>     GetGlyphData()      { return m_GlyphData;      }
			TextBoundingBox& GetBoundingBox()    { return m_BoundingBox;    }

		private:
			ref<VertexArray>    m_GlyphBoundsVAO;
			ref<Texture>        m_GlyphData;
			TextBoundingBox m_BoundingBox;
		};

	}
}