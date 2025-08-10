#pragma once
#include <string>
#include <map>

#include <Enigma/Renderer/Primitives/Text.h>

namespace Enigma {
	namespace Serialization {

		/*enum class FontType {
			None = 0,
			TrueType,
			OpenType
		};*/
		class Font {
		public:
			static Font* Load(const std::string& filename);
			virtual ~Font() { }

			virtual Renderer::Glyph GetGlyph(uint16_t unicode) = 0;
			virtual int GetVerticalSpacing() = 0;
			virtual int GetSpaceHorizontalAdvance() = 0;

			Renderer::Text* GenerateText(const std::string& text, float scale = 0.0005);

		private:
			struct Vertex {
				glm::vec3 position;
				glm::vec2 texCoord;
				float charID;
			};
			inline static std::vector<Renderer::DataType> s_VertexLayout = {
				Renderer::DataType::Float3,
				Renderer::DataType::Float2,
				Renderer::DataType::Float
			};
			inline static std::vector<Vertex> s_QuadVertices = {
				{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
				{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
				{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
				{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }
			};
			inline static std::vector<unsigned int> s_QuadIndices = {
				0,1,3,
				1,2,3
			};
		};
	}
}