#include "Enigma/EnigmaPCH.h"
#include "Enigma/Renderer/Shader.h"
#include "Enigma/Renderer/Renderer2D.h"

#include "Enigma/Core/Process/Application.h"

using namespace Enigma::Engine::ECS;

namespace Enigma::Renderer {
	ref<Renderer2D> Renderer2D::Create(Renderer::API renderAPI, const Engine::Surface& surface) { return CreateRef<Renderer2D>(renderAPI, surface); }
	Renderer2D::Renderer2D(Renderer::API renderAPI, const Engine::Surface& surface) : m_RenderAPI(renderAPI), m_Surface(surface) {
		// Load shaders
		ShaderConfig quadShaderConfig;
		quadShaderConfig.stages = {
			{ ShaderStageType::Vertex, "../Enigma/DefaultShaders/QuadShader.vert" },
			{ ShaderStageType::Fragment, "../Enigma/DefaultShaders/QuadShader.frag" },
		};
		m_QuadShader = Shader::Create(quadShaderConfig);

		ShaderConfig circleShaderConfig;
		circleShaderConfig.stages = {
			{ ShaderStageType::Vertex, "../Enigma/DefaultShaders/CircleShader.vert" },
			{ ShaderStageType::Fragment, "../Enigma/DefaultShaders/CircleShader.frag" },
		};
		m_CircleShader = Shader::Create(circleShaderConfig);

		ShaderConfig postProcShaderConfig;
		postProcShaderConfig.stages = {
			{ ShaderStageType::Vertex, "../Enigma/DefaultShaders/PostProcessShader.vert" },
			{ ShaderStageType::Fragment, "../Enigma/DefaultShaders/PostProcessShader.frag" }
		};
		m_PostProcShader = Shader::Create(postProcShaderConfig);

		// Create framebuffers
		FrameBufferConfig frameBufferConfig;
		frameBufferConfig.width = static_cast<u32>(m_Surface.scale.x);
		frameBufferConfig.height = static_cast<u32>(m_Surface.scale.y);

		// Main attachment
		Attachment colorAttachment;
		colorAttachment.type = AttachmentType::ColorAttachment;
		colorAttachment.internalFormat = TexFormat::RGBA;
		colorAttachment.format = TexFormat::RGBA;

		// Entity IDs get written to this buffer
		Attachment entityIDAttachment;
		entityIDAttachment.type           = AttachmentType::ColorAttachment;
		entityIDAttachment.internalFormat = TexFormat::RED32I;
		entityIDAttachment.format         = TexFormat::RED_INT;

		frameBufferConfig.attachments = { colorAttachment, entityIDAttachment };

		m_FrameBuffer = FrameBuffer::Create(frameBufferConfig);

		if (m_Surface.frame != nullptr) {
			colorAttachment.output = m_Surface.frame;
			frameBufferConfig.attachments = { colorAttachment };
			m_OutputBuffer = FrameBuffer::Create(frameBufferConfig);
		}

		// Init quad if needed
		if (s_Quad == nullptr) InitQuad();

		m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0, Usage::DynamicDraw);
		m_ModelUniformBuffer = UniformBuffer::Create(sizeof(ModelData), 1, Usage::DynamicDraw);

		// Config render api
		Core::Application::UseRenderAPI(m_RenderAPI);
		RenderAPI::SetClearColor({ 0,0,0,1 });
		RenderAPI::SetClearMask(ColorBufferBit | DepthBufferBit);
		RenderAPI::SetDrawMode(DrawMode::Triangles);
		RenderAPI::SetViewport(static_cast<u32>(m_Surface.scale.x), static_cast<u32>(m_Surface.scale.y));
	}
	Renderer2D::~Renderer2D() { }

	void Renderer2D::InitQuad()	{
		s_Quad = Renderer::VertexArray::Create();
		s_Quad->Bind();

		ref<Renderer::VertexBuffer> vertexBuffer = Renderer::VertexBuffer::Create(s_VertexLayout, Renderer::Usage::StaticDraw);
		vertexBuffer->SetData(&s_QuadVertices[0], s_QuadVertices.size() * sizeof(s_QuadVertices[0]));
		vertexBuffer->InitAttribs();
		s_Quad->AttachBuffer(vertexBuffer);

		ref<Renderer::IndexBuffer> indexBuffer = Renderer::IndexBuffer::Create(Renderer::DataType::UnsignedInt, Renderer::Usage::StaticDraw);
		indexBuffer->SetData(&s_QuadIndices[0], s_QuadIndices.size() * sizeof(s_QuadIndices[0]));
		s_Quad->AttachBuffer(indexBuffer);

		s_Quad->Unbind();
	}

	void Renderer2D::Resize(u32 width, u32 height) {
		Renderer::RenderAPI::SetViewport(width, height);
		m_FrameBuffer->Resize(width, height);
		if (m_OutputBuffer != nullptr)  m_OutputBuffer->Resize(width, height);
		if (m_CurrentCamera != nullptr) m_CurrentCamera->Resize(width, height);
	}

	void Renderer2D::StartScene(const ref<Engine::ECS::ECS> ecs, const ref<Camera>& camera) {
		m_CurrentECS = ecs;
		m_CurrentCamera = camera;

		Core::Application::UseRenderAPI(m_RenderAPI);
		Renderer::RenderAPI::Clear();

		CameraData cameraData = m_CurrentCamera->GetData();
		m_CameraUniformBuffer->SetData(&cameraData, sizeof(CameraData), 0);

		// Bind and clear frame buffer
		m_FrameBuffer->Bind();
		Renderer::RenderAPI::Clear();
	}
	void Renderer2D::EndScene() {
		m_FrameBuffer->Unbind();

		// If the surface passed to the renderer has a frame then render to that frame with the output buffer
		if (m_OutputBuffer != nullptr) {
			m_OutputBuffer->Bind();
			Renderer::RenderAPI::Clear();
		}

		m_PostProcShader->Bind();
		s_Quad->Bind();

		// Run the frame through the post process shader
		ref<Renderer::Texture> frame = m_FrameBuffer->GetColorAttachment(0);

		frame->Bind();
		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
		frame->Unbind();

		s_Quad->Unbind();
		m_PostProcShader->Unbind();

		if (m_OutputBuffer != nullptr) m_OutputBuffer->Unbind();
		m_CurrentECS = nullptr;
	}

	void Renderer2D::DrawEntity(EntityID id) {
		// Make sure entity has a render component
		bool hasQuad = m_CurrentECS->HasComponent<QuadRendererComponent>(id);
		bool hasCircle = m_CurrentECS->HasComponent<CircleRendererComponent>(id);
		if (!hasQuad && !hasCircle) return;
		
		// Get world transform matrix
		TransformComponent& transformComponent = m_CurrentECS->GetComponent<TransformComponent>(id);
		glm::mat4 transformMatrix = transformComponent.GetWorldMatrix(m_CurrentECS);

		// Draw a quad if entity has a quad component
		if (hasQuad) {
			QuadRendererComponent& quadComponent = m_CurrentECS->GetComponent<QuadRendererComponent>(id);
			DrawColoredQuad(transformMatrix, quadComponent.tint, id);
		}

		// Draw a circle if entity has a circle component
		if (hasCircle) {
			CircleRendererComponent& circleComponent = m_CurrentECS->GetComponent<CircleRendererComponent>(id);
			DrawColoredCircle(transformMatrix, circleComponent.tint, circleComponent.thickness, circleComponent.fade, id);
		}
	}
	void Renderer2D::DrawColoredQuad(const glm::mat4& transform, const glm::vec3& tint, EntityID id) {
		// Configure quad
		ModelData data;
		data.model = transform;
		data.tint = { tint, 1.0f };
		data.entityID = static_cast<i32>(id);
		data.thickness = 0.0f;
		data.fade = 0.0f;
		m_ModelUniformBuffer->SetData(&data, sizeof(ModelData), 0);

		// Draw quad
		m_QuadShader->Bind();
		s_Quad->Bind();
		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
		s_Quad->Unbind();
		m_QuadShader->Unbind();
	}
	void Renderer2D::DrawColoredCircle(const glm::mat4& transform, const glm::vec3& tint, f32 thickness, f32 fade, Engine::ECS::EntityID id) {
		// Configure quad
		ModelData data;
		data.model = transform;
		data.tint = { tint, 1.0f };
		data.entityID = static_cast<i32>(id);
		data.thickness = thickness;
		data.fade = fade;
		m_ModelUniformBuffer->SetData(&data, sizeof(ModelData), 0);

		// Draw quad
		m_CircleShader->Bind();
		s_Quad->Bind();
		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
		s_Quad->Unbind();
		m_CircleShader->Unbind();
	}

	void Renderer2D::ColoredQuadSystem(EntityID entityID, TransformComponent& transform, QuadRendererComponent& quad)
	{
		glm::mat4 model = transform.GetWorldMatrix(m_CurrentECS);

		glm::vec4 tint = { quad.tint, 1.0f };

		// Draw quad
		ModelData data;
		data.model = model;
		data.tint = tint;
		data.entityID = static_cast<i32>(entityID);
		m_ModelUniformBuffer->SetData(&data, sizeof(ModelData), 0);
		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
	}
}