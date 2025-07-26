#pragma once
#include "Enigma/Renderer/Shader.h"
#include "Enigma/Renderer/Camera.h"
#include "Enigma/Renderer/VertexArray.h"

namespace Enigma {
	namespace Renderer {

		struct Render2DConfig {
			API renderAPI;
			Engine::Surface surface;
			Shader* mainShader = nullptr; // if nullptr then the default main shader will be used
			Shader* postProcShader = nullptr; // if nullptr thrn the default postProcShader will be used
		};

		class Render2D {
		public:
			Render2D(const Render2DConfig& config);

			void Resize(int width, int height);

			void StartFrame(OrthographicCamera* camera);
			void EndFrame();

			void DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, const glm::vec4& tint);
			void DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, Texture* texture, const glm::vec4& tint);

			Shader* GetMainShader() { return m_MainShader; }
			Shader* GetPostProcShader() { return m_PostProcShader; }

		private:
			static Shader* LoadDefaultMainShader();
			static Shader* LoadDefaultPostProcShader();

		private:
			struct DrawCall {
				OrthographicCamera* camera;
				Texture* texture;

				glm::mat4 model;
				glm::vec4 tint;
			};

			OrthographicCamera* m_CurrentCamera;

			FrameBuffer* m_FrameBuffer;
			FrameBuffer* m_OutputBuffer;

			Shader* m_MainShader;
			Shader* m_PostProcShader;

			Texture* m_BlankTexture; // Used for anything with no texture

			std::vector<DrawCall> m_DrawCalls;

			API m_RenderAPI;
			Engine::Surface m_Surface;

		private:
			struct Vertex {
				glm::vec3 position;
				glm::vec2 textureCoord;
			};

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
		};

	}
}