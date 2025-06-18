#pragma once
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

#include <map>
#include <glm/glm.hpp>

namespace Enigma {
	namespace Renderer {

		struct Render2DConfig {
			std::string mainShaderPath = "ExampleApp/assets/shader2D";

			bool batch = false; // Tells the renderer if it should batch quads together
		};

		class Render2D {
		public:
			static void Init(const Render2DConfig& config);

			static void Resize(int width, int height);

			static void StartFrame(Camera* camera);
			static void EndFrame();

			static void DrawQuad(const glm::vec2& position, float depth, const glm::vec3& tint = { 1.0f, 1.0f, 1.0f }) { DrawQuad(position, depth, { 1,1 }, 0.0f, tint); }
			static void DrawQuad(const glm::vec2& position, float depth, const glm::vec2& scale, const glm::vec3& tint = { 1.0f, 1.0f, 1.0f }) { DrawQuad(position, depth, scale, 0.0f, tint); }
			static void DrawQuad(const glm::vec2& position, float depth, const glm::vec2& scale, float rotation, const glm::vec3& tint = { 1.0f, 1.0f, 1.0f });

			static void DrawQuad(const glm::vec2& position, float depth, Texture* texture, const glm::vec3& tint = { 1.0f, 1.0f, 1.0f }) { DrawQuad(position, depth, { 1,1 }, 0.0f, texture, tint); }
			static void DrawQuad(const glm::vec2& position, float depth, const glm::vec2& scale, Texture* texture, const glm::vec3& tint = { 1.0f, 1.0f, 1.0f }) { DrawQuad(position, depth, scale, 0.0f, texture, tint); }
			static void DrawQuad(const glm::vec2& position, float depth, const glm::vec2& scale, float rotation, Texture* texture, const glm::vec3& tint = { 1.0f, 1.0f, 1.0f });

		private:
			
			struct Vertex {
				glm::vec3 position;
				glm::vec2 textureCoord;
			};

			struct DrawCall {
				Camera* camera;
				Texture* texture;
				glm::mat4 model;
				glm::vec3 tint;

				static uint64_t hash(Camera* camera, Texture* texture) {
					uint32_t v1 = camera->GetID().index;
					uint32_t v2 = texture->GetID().index;
					uint32_t t1 = v1 ^ camera->GetID().generation;
					t1 << v2;
					uint32_t t2 = v2 ^ texture->GetID().generation;
					t2 << v1;
					uint64_t rslt = (t1 << 31) | t2;
					return rslt;
				}
			};
			static void SubmitDrawCall(const DrawCall& call);

			struct Data {
				Render2DConfig config;

				Camera* currentCamera;
				Shader* mainShader;
				
				Texture* emptyTexture; // used for anything with no texture.

				std::map<uint64_t, int> drawCallLookup; // used to order the draw calls
				std::vector<std::vector<DrawCall>> drawCalls;
			};

		private:
			inline static std::vector<DataType> s_VertexLayout = {
				DataType::Float3,
				DataType::Float2
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
			inline static VertexArray* s_Quad;

		private:
			inline static Data* s_Data;
		};

	}
}