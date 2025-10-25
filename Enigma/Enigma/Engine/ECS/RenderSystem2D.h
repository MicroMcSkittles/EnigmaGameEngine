#pragma once
#include "Enigma/Engine/ECS/EntityComponentSystem.h"
#include "Enigma/Engine/ECS/Components.h"

#include "Enigma/Renderer/Shader.h"
#include "Enigma/Renderer/Camera.h"
#include "Enigma/Renderer/Buffer.h"
#include "Enigma/Renderer/VertexArray.h"

namespace Enigma::Engine::ECS {

	struct RenderSystem2DConfig {
		Renderer::API renderAPI;
		Engine::Surface surface;

		ref<Renderer::Shader> mainShader; // If nullptr then the default main shader will be used
		ref<Renderer::Shader> postProcShader; // If nullptr thrn the default postProcShader will be used
	};

	class RenderSystem2D {
	public:
		static ref<RenderSystem2D> Create(const RenderSystem2DConfig& config, ref<ECS> ecs);
		RenderSystem2D(const RenderSystem2DConfig& config, ref<ECS> ecs);
		~RenderSystem2D();

		void Resize(i32 width, i32 height);

		void StartFrame(ref<Renderer::OrthographicCamera> camera);
		void EndFrame();

	private:
		void ColoredQuadSystem(Transform& transform, ColoredQuad& quad);
		void TexturedQuadSystem(Transform& transform, TexturedQuad& quad);

		void ColoredCircleSystem(Transform& transform, ColoredCircle& circle);
		void TexturedCircleSystem(Transform& transform, TexturedCircle& circle);

	private:
		ref<ECS> m_ECS;

		ref<Renderer::OrthographicCamera> m_CurrentCamera;

		// Framebuffers
		ref<Renderer::FrameBuffer> m_FrameBuffer;
		ref<Renderer::FrameBuffer> m_OutputBuffer;

		// Shaders
		ref<Renderer::Shader> m_MainShader;
		ref<Renderer::Shader> m_CircleShader;
		ref<Renderer::Shader> m_PostProcShader;

		// Other
		ref<Renderer::Texture> m_BlankTexture; // Used for anything with no texture

		RenderSystem2DConfig m_Config;

	private:
		struct Vertex {
			glm::vec3 position;
			glm::vec2 texCoord;
		};

		// Default Quad
		inline static std::vector<Renderer::DataType> s_VertexLayout = {
			Renderer::DataType::Float3,
			Renderer::DataType::Float2
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

		inline static ref<Renderer::VertexArray> s_Quad;
	};

}