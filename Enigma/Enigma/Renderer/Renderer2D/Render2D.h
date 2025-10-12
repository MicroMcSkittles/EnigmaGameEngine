#pragma once
#include "Enigma/Renderer/Shader.h"
#include "Enigma/Renderer/Camera.h"
#include "Enigma/Renderer/VertexArray.h"
#include "Enigma/Renderer/Primitives/Vertex.h"
#include "Enigma/Renderer/Primitives/Text.h"

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
			void SetCamera(OrthographicCamera* camera);

			void DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, const glm::vec4& tint);
			void DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, Texture* texture, const glm::vec4& tint);
			
			void DrawLineQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, float thickness, int depth, const glm::vec4& tint);
			void DrawLineQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, float thickness, int depth, Texture* texture, const glm::vec4& tint);

			void DrawCircle(const glm::vec2& position, float radius, int depth, const glm::vec4& tint);
			void DrawCircle(const glm::vec2& position, float radius, int depth, Texture* texture, const glm::vec4& tint);
			
			void DrawLineCircle(const glm::vec2& position, float radius, float thickness, int depth, const glm::vec4& tint);
			void DrawLineCircle(const glm::vec2& position, float radius, float thickness, int depth, Texture* texture, const glm::vec4& tint);

			void DrawText(Text* text, const glm::vec2& position, float scale, float rotation, int depth, const glm::vec4& tint);
			void DrawText(Text* text, const glm::vec2& position, float scale, float rotation, int depth, Texture* texture, const glm::vec4& tint);

			Shader* GetMainShader() { return m_MainShader; }
			Shader* GetPostProcShader() { return m_PostProcShader; }

			Texture* GetBlankTexture() { return m_BlankTexture; }

		private:
			enum class StencilType {
				None = 0,
				LineQuad,
				Circle,
				LineCircle,
				Text
			};
			struct LineQuadStencil {
				int id;
				glm::mat4 transform;
				glm::vec2 bounds;
				float thickness;
			};
			struct CircleStencil {
				int id;
				glm::mat4 transform;
			};
			struct LineCircleStencil {
				int id;
				glm::mat4 transform;
				float thickness;
			};
			struct TextStencil {
				int id;
				glm::mat4 transform;
				Text* text;
			};

		private:

			static Shader* LoadCircleStencilShader();
			static Shader* LoadLineCircleStencilShader();
			static Shader* LoadLineQuadStencilShader();
			static Shader* LoadTextStencilShader();

			void DrawLineQuadStencil(LineQuadStencil* stencil);
			void DrawCircleStencil(CircleStencil* stencil);
			void DrawLineCircleStencil(LineCircleStencil* stencil);
			void DrawTextStencil(TextStencil* stencil);

			void Submit(const glm::mat4& transform, const glm::vec4& tint, StencilType stencilType, void* stencil, Texture* texture);

			// Checks if a quad is on the render target, used for culling
			bool OnScreen(const glm::vec2& position, const glm::vec2& scale);

		private:
		public:

			struct DrawCall {
				glm::mat4 transform;
				glm::vec4 tint;
				int       stencilID;
			};
			struct Batch {
				OrthographicCamera* camera;
				Texture*            texture;
				StencilType         stencilType;
				Shader*             stencilShader;

				std::vector<DrawCall> drawCalls;
				std::vector<void*> stencils;

				static uint64_t Hash(const Batch& batch);
			};	

			// Camera variables
			OrthographicCamera* m_CurrentCamera;
			ViewBox             m_CameraWorldBounds;

			// Framebuffers
			FrameBuffer* m_FrameBuffer;
			FrameBuffer* m_StencilBuffer;
			FrameBuffer* m_OutputBuffer;

			// Shaders
			Shader* m_MainShader;
			Shader* m_CircleStencilShader;
			Shader* m_LineCircleStencilShader;
			Shader* m_LineQuadStencilShader;
			Shader* m_TextStencilShader;
			Shader* m_PostProcShader;

			// Default Textures
			Texture* m_BlankTexture; // Used for anything with no texture
			Texture* m_StencilTexture;

			int m_CurrentStencilID;
			std::map<uint64_t, Batch> m_Batches;

			// Other
			API m_RenderAPI;
			Engine::Surface m_Surface;

		private:
			// Default Quad
			inline static std::vector<DataType> s_VertexLayout = {
				DataType::Float3,
				DataType::Float2
			};
			inline static std::vector<PrimitiveVertex> s_QuadVertices = {
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