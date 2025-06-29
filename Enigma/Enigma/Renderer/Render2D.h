#pragma once
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/VertexArray.h"

namespace Enigma {
	namespace Renderer {

		struct Render2DConfig : public RendererConfig {
			Render2DConfig() { 
				type = RendererType::Render2D;
			};

			Shader* mainShader;
			Shader* postProcShader;
		};

		class Render2D : public Renderer {
		public:
			Render2D(const Render2DConfig& config);

			static void Resize(int width, int height) { ((Render2D*)s_CurrentContext)->ResizeImpl(width, height); }

			static void StartFrame(OrthographicCamera* camera) { ((Render2D*)s_CurrentContext)->StartFrameImpl(camera); }
			static void EndFrame() { ((Render2D*)s_CurrentContext)->EndFrameImpl(); }

			static void DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, const glm::vec4& tint) { ((Render2D*)s_CurrentContext)->DrawQuadImpl(position, scale, rotation, depth, tint); }
			static void DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, Texture* texture, const glm::vec4& tint) { ((Render2D*)s_CurrentContext)->DrawQuadImpl(position, scale, rotation, depth, texture, tint); }

		private:
			void ResizeImpl(int width, int height);

			void StartFrameImpl(OrthographicCamera* camera);
			void EndFrameImpl();

			void DrawQuadImpl(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, const glm::vec4& tint);
			void DrawQuadImpl(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, Texture* texture, const glm::vec4& tint);

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