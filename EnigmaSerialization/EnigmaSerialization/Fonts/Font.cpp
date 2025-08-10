#include "EnigmaSerialization/Fonts/Font.h"
#include "EnigmaSerialization/Fonts/FontTTF.h"
#include "EnigmaSerialization/Utils.h"

#include <filesystem>
#include <Enigma/Core/Core.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Enigma {
	namespace Serialization {
		Font* Font::Load(const std::string& filename)
		{
			std::string extension = std::filesystem::path(filename).extension().string();
			if (extension == ".ttf") return new TTF::FontTTF(filename);
			else LOG_WARNING("Unknown font file type ( %s )", extension.c_str());
			return nullptr;
		}

		Renderer::Text* Font::GenerateText(const std::string& text, float scale)
		{
			// Setup bounding box
			Renderer::TextBoundingBox boundingBox;
			boundingBox.max = { 0,0 };
			boundingBox.min = { 0,0 };

			std::vector<Vertex>   vertices;
			std::vector<uint32_t> indices;

			// Get spaceing data
			float verticalSpacing        = (float)GetVerticalSpacing()        * scale;
			float spaceHorizontalAdvance = (float)GetSpaceHorizontalAdvance() * scale;

			glm::vec2 glyphOffset = { 0,0 };
			std::vector<Renderer::Glyph> glyphs;
			std::map<uint16_t, uint32_t> glyphLookupTable;

			// Loop through each character in text
			for (int charID = 0; charID < text.size(); ++charID) {
				char c = text[charID];
				// Process spaces
				if (c == ' ') {
					glyphOffset.x += spaceHorizontalAdvance;
					continue;
				}
				// process new lines
				if (c == '\n') {
					glyphOffset.x = 0;
					glyphOffset.y -= verticalSpacing;
					continue;
				}
				
				// If character glyph hasn't been used yet then get the glyph and map the unicode to the glyph index
				if (!glyphLookupTable.count(c)) {
					glyphLookupTable.insert({ c, glyphs.size() });
					glyphs.push_back(GetGlyph(c));
				}

				Renderer::Glyph* glyph = &glyphs[glyphLookupTable[c]];

				glm::vec2 glyphPosition = (glyph->boundingBox.position * scale) + glyphOffset;
				
				// Calculate the min and max bounds for the entire text object
				if ((glyph->boundingBox.max.x * scale) + glyphOffset.x > boundingBox.max.x) boundingBox.max.x = (glyph->boundingBox.max.x * scale) + glyphOffset.x;
				if ((glyph->boundingBox.max.y * scale) + glyphOffset.y > boundingBox.max.y) boundingBox.max.y = (glyph->boundingBox.max.y * scale) + glyphOffset.y;
				if ((glyph->boundingBox.min.x * scale) + glyphOffset.x < boundingBox.min.x) boundingBox.min.x = (glyph->boundingBox.min.x * scale) + glyphOffset.x;
				if ((glyph->boundingBox.min.y * scale) + glyphOffset.y < boundingBox.min.y) boundingBox.min.y = (glyph->boundingBox.min.y * scale) + glyphOffset.y;
				
				// Transform glyph
				glm::mat4 glyphTransform = glm::mat4(1.0f);
				glyphTransform = glm::translate(glyphTransform, { glyphPosition, 0 });
				glyphTransform = glm::scale(glyphTransform, { glyph->boundingBox.size * scale, 1 });

				// Create glyph bounding box vertex and index data
				for (Vertex& vertex : s_QuadVertices) {
					Vertex v = vertex;
					v.position = glyphTransform * glm::vec4(v.position, 1);
					v.charID = glyphLookupTable[c];
					vertices.push_back(v);
				}
				for (uint32_t& index : s_QuadIndices) {
					indices.push_back(index + (vertices.size() - s_QuadVertices.size()));
				}

				// Advance the glyph offset horizontaly
				glyphOffset.x += (float)glyph->horizontalAdvance * scale;
			}

			// Calculate position and size of the text bounding box
			boundingBox.position = {
				(boundingBox.min.x + boundingBox.max.x) / 2.0f,
				(boundingBox.min.y + boundingBox.max.y) / 2.0f
			};
			boundingBox.size = {
				glm::distance(boundingBox.min.x, boundingBox.max.x) / 2.0f,
				glm::distance(boundingBox.min.y, boundingBox.max.y) / 2.0f
			};

			// Create text vao, this contains the bounding boxes for all the glyphs
			Renderer::VertexArray* glyphBoundsVAO = Renderer::VertexArray::Create();
			glyphBoundsVAO->Bind();

			Renderer::VertexBuffer* vbo = Renderer::VertexBuffer::Create(s_VertexLayout, Renderer::Usage::StaticDraw);
			vbo->SetData(&vertices[0], sizeof(Vertex) * vertices.size());
			vbo->InitAttribs();
			glyphBoundsVAO->AttachBuffer(vbo);

			Renderer::IndexBuffer* ebo = Renderer::IndexBuffer::Create(Renderer::DataType::UnsignedInt, Renderer::Usage::StaticDraw);
			ebo->SetData(&indices[0], 4 * indices.size());
			glyphBoundsVAO->AttachBuffer(ebo);

			glyphBoundsVAO->Unbind();

			// Size of the elements that make up the glyph data
			const uint32_t glyphHeaderSize  = 2;
			const uint32_t glyphContourSize = 2;
			const uint32_t glyphPointSize   = 2;

			// Figure out how large the glyph data texture needs to be
			uint32_t glyphDataSize = 0;
			std::vector<uint32_t> individualGlyphSizes(glyphs.size());
			
			// Loop through all glyphs
			for (uint16_t glyphIndex = 0; glyphIndex < glyphs.size(); ++glyphIndex) {
				Renderer::Glyph& glyph = glyphs[glyphIndex];

				uint32_t individualGlyphSize = 0;
				individualGlyphSize += glyphHeaderSize;
				individualGlyphSize += glyph.contours.size() * glyphContourSize;
				individualGlyphSize += glyph.points.size()   * glyphPointSize;
				glyphDataSize       += individualGlyphSize;
				individualGlyphSizes[glyphIndex] = individualGlyphSize;
			}

			// Allocate memory for the glyph data
			std::vector<float> glyphData(glyphDataSize);

			// Points to where we are in the glyphData vector
			uint64_t pointer = 0;

			// Loop through each glyph
			for (uint16_t glyphIndex = 0; glyphIndex < glyphs.size(); ++glyphIndex) {
				Renderer::Glyph& glyph = glyphs[glyphIndex];

				// Store glyph header
				uint32_t glyphSize = individualGlyphSizes[glyphIndex];
				glyphData[pointer++] = glyphSize;
				glyphData[pointer++] = glyph.contours.size();

				// Store contours
				for (Renderer::GlyphContour& contour : glyph.contours) {
					glyphData[pointer++] = contour.start;
					glyphData[pointer++] = contour.end;
				}

				// Store Points, each point gets converted to a range -1 to 1 based on the glyphs bounding box min and max points
				for (Renderer::TextPoint& point : glyph.points) {
					glyphData[pointer++] = ((point.position.x - glyph.boundingBox.min.x) / (glyph.boundingBox.max.x - glyph.boundingBox.min.x)) * 2 - 1;
					glyphData[pointer++] = ((point.position.y - glyph.boundingBox.min.y) / (glyph.boundingBox.max.y - glyph.boundingBox.min.y)) * 2 - 1;
				}
			}

			// Configure glyph data texture
			Renderer::TextureConfig textureConfig;
			textureConfig.width = glyphDataSize;
			textureConfig.height = 1;

			textureConfig.dataType = Renderer::DataType::Float;
			textureConfig.format = Renderer::TexFormat::RED;
			textureConfig.internalFormat = Renderer::TexFormat::RED32F;

			textureConfig.minFilter = Renderer::TexFilterMode::Nearest;
			textureConfig.magFilter = Renderer::TexFilterMode::Nearest;

			textureConfig.data = glyphData.data();

			// Create glyph data texture
			Renderer::Texture* glyphDataTexture = Renderer::Texture::Create(textureConfig);

			// Create text object
			Renderer::Text* textObject = new Renderer::Text(glyphBoundsVAO, glyphDataTexture, boundingBox);
			return textObject;
		}
	}
}