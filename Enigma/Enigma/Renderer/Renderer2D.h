#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/Shader.h"
#include "Enigma/Renderer/Buffer.h"
#include "Enigma/Renderer/Camera.h"
#include "Enigma/Renderer/VertexArray.h"
#include "Enigma/Engine/Surface.h"
#include "Enigma/Engine/ECS/Components.h"

namespace Enigma::Renderer {

	class Renderer2D {
	public:
		static ref<Renderer2D> Create(Renderer::API renderAPI, const Engine::Surface& surface);
		Renderer2D(Renderer::API renderAPI, const Engine::Surface& surface);
		~Renderer2D();

		void Resize(u32 width, u32 height);

		void StartScene(const ref<Engine::ECS::ECS> ecs, const ref<Camera>& camera);
		void EndScene();

		ref<FrameBuffer> GetFrameBuffer() { return m_FrameBuffer; }
		Renderer::API GetRenderAPI() { return m_RenderAPI; }

	private:
		static void InitQuad();
		void ColoredQuadSystem(Engine::ECS::EntityID entityID, Engine::ECS::Transform& transform, Engine::ECS::ColoredQuad& quad);

	private:
		Renderer::API m_RenderAPI;
		Engine::Surface m_Surface;

		ref<Engine::ECS::ECS> m_CurrentECS;
		ref<Camera> m_CurrentCamera;
		ref<UniformBuffer> m_CameraUniformBuffer;
		ref<UniformBuffer> m_ModelUniformBuffer;

		ref<FrameBuffer> m_FrameBuffer;
		ref<FrameBuffer> m_OutputBuffer;

		ref<Shader> m_MainShader;
		ref<Shader> m_PostProcShader;

	private:
		struct ModelData {
			glm::mat4 model;
			glm::vec4 tint;
			i32 entityID;
		};

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