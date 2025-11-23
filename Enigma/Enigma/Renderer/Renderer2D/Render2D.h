#pragma once
#include "Enigma/Renderer/Shader.h"
#include "Enigma/Renderer/Camera.h"
#include "Enigma/Renderer/VertexArray.h"
#include "Enigma/Renderer/Primitives/Vertex.h"
#include "Enigma/Renderer/Primitives/Text.h"

namespace Enigma::Renderer {

	struct Render2DConfig {
		API renderAPI;
		Engine::Surface surface;
		ref<Shader> mainShader = nullptr; // if nullptr then the default main shader will be used
		ref<Shader> postProcShader = nullptr; // if nullptr thrn the default postProcShader will be used
	};

	class Render2D {
	public:
		Render2D(const Render2DConfig& config);

		void Resize(i32 width, i32 height);

		void StartFrame(ref<OrthographicCamera> camera);
		void EndFrame();
		void SetCamera(ref<OrthographicCamera> camera);

		void DrawQuad(const glm::vec2& position, const glm::vec2& scale, f32 rotation, i32 depth, const glm::vec4& tint);
		void DrawQuad(const glm::vec2& position, const glm::vec2& scale, f32 rotation, i32 depth, ref<Texture> texture, const glm::vec4& tint);
		
		void DrawLineQuad(const glm::vec2& position, const glm::vec2& scale, f32 rotation, f32 thickness, i32 depth, const glm::vec4& tint);
		void DrawLineQuad(const glm::vec2& position, const glm::vec2& scale, f32 rotation, f32 thickness, i32 depth, ref<Texture> texture, const glm::vec4& tint);

		void DrawCircle(const glm::vec2& position, f32 radius, i32 depth, const glm::vec4& tint);
		void DrawCircle(const glm::vec2& position, f32 radius, i32 depth, ref<Texture> texture, const glm::vec4& tint);
		
		void DrawLineCircle(const glm::vec2& position, f32 radius, f32 thickness, i32 depth, const glm::vec4& tint);
		void DrawLineCircle(const glm::vec2& position, f32 radius, f32 thickness, i32 depth, ref<Texture> texture, const glm::vec4& tint);

		void DrawText(Text* text, const glm::vec2& position, f32 scale, f32 rotation, i32 depth, const glm::vec4& tint);
		void DrawText(Text* text, const glm::vec2& position, f32 scale, f32 rotation, i32 depth, ref<Texture> texture, const glm::vec4& tint);

		ref<Shader> GetMainShader() { return m_MainShader; }
		ref<Shader> GetPostProcShader() { return m_PostProcShader; }

		ref<Texture> GetBlankTexture() { return m_BlankTexture; }

	private:
		enum class StencilType {
			None = 0,
			LineQuad,
			Circle,
			LineCircle,
			Text
		};
		struct LineQuadStencil {
			i32 id;
			glm::mat4 transform;
			glm::vec2 bounds;
			f32 thickness;
		};
		struct CircleStencil {
			i32 id;
			glm::mat4 transform;
		};
		struct LineCircleStencil {
			i32 id;
			glm::mat4 transform;
			f32 thickness;
		};
		struct TextStencil {
			i32 id;
			glm::mat4 transform;
			Text* text;
		};

	private:

		static ref<Shader> LoadCircleStencilShader();
		static ref<Shader> LoadLineCircleStencilShader();
		static ref<Shader> LoadLineQuadStencilShader();
		static ref<Shader> LoadTextStencilShader();

		void DrawLineQuadStencil(LineQuadStencil* stencil);
		void DrawCircleStencil(CircleStencil* stencil);
		void DrawLineCircleStencil(LineCircleStencil* stencil);
		void DrawTextStencil(TextStencil* stencil);

		void Submit(const glm::mat4& transform, const glm::vec4& tint, StencilType stencilType, void* stencil, ref<Texture> texture);

		// Checks if a quad is on the render target, used for culling
		bool OnScreen(const glm::vec2& position, const glm::vec2& scale);

	private:
	public:

		struct DrawCall {
			glm::mat4 transform;
			glm::vec4 tint;
			i32       stencilID;
		};
		struct Batch {
			ref<OrthographicCamera> camera;
			ref<Texture>            texture;
			StencilType             stencilType;
			ref<Shader>             stencilShader;

			std::vector<DrawCall> drawCalls;
			std::vector<void*> stencils;

			static u64 Hash(const Batch& batch);
		};	

		// Camera variables
		ref<OrthographicCamera> m_CurrentCamera;
		ViewBox                 m_CameraWorldBounds;

		// Framebuffers
		ref<FrameBuffer> m_FrameBuffer;
		ref<FrameBuffer> m_StencilBuffer;
		ref<FrameBuffer> m_OutputBuffer;

		// Shaders
		ref<Shader> m_MainShader;
		ref<Shader> m_CircleStencilShader;
		ref<Shader> m_LineCircleStencilShader;
		ref<Shader> m_LineQuadStencilShader;
		ref<Shader> m_TextStencilShader;
		ref<Shader> m_PostProcShader;

		// Default Textures
		ref<Texture> m_BlankTexture; // Used for anything with no texture
		ref<Texture> m_StencilTexture;

		i32 m_CurrentStencilID;
		std::map<u64, Batch> m_Batches;

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
		inline static std::vector<u32> s_QuadIndices = {
			0,1,3,
			1,2,3
		};
		inline static ref<VertexArray> s_Quad;
	};
}