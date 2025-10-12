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

		Renderer::Shader* mainShader = nullptr; // If nullptr then the default main shader will be used
		Renderer::Shader* postProcShader = nullptr; // If nullptr thrn the default postProcShader will be used
	};

	class RenderSystem2D {
	public:
		RenderSystem2D(const RenderSystem2DConfig& config, ECS* ecs);
		~RenderSystem2D();

		void Resize(int width, int height);

		void StartFrame(Renderer::OrthographicCamera* camera);
		void EndFrame();

	private:
		void ColoredQuadSystem(Transform& transform, ColoredQuad& quad);
		void TexturedQuadSystem(Transform& transform, TexturedQuad& quad);

		void ColoredCircleSystem(Transform& transform, ColoredCircle& circle);
		void TexturedCircleSystem(Transform& transform, TexturedCircle& circle);

	private:
		ECS* m_ECS;

		Renderer::OrthographicCamera* m_CurrentCamera;

		// Framebuffers
		Renderer::FrameBuffer* m_FrameBuffer;
		Renderer::FrameBuffer* m_OutputBuffer;

		// Shaders
		Renderer::Shader* m_MainShader;
		Renderer::Shader* m_CircleShader;
		Renderer::Shader* m_PostProcShader;

		// Other
		Renderer::Texture* m_BlankTexture; // Used for anything with no texture

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

		inline static Renderer::VertexArray* s_Quad;
	};

}