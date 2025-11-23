#include "Enigma/EnigmaPCH.h"
#include "Enigma/Engine/ECS/RenderSystem2D.h"
#include "Enigma/Renderer/DefaultShaders.h"
#include "Enigma/Core/Process/Application.h"

namespace Enigma::Engine::ECS {
	ref<RenderSystem2D> RenderSystem2D::Create(const RenderSystem2DConfig& config, ref<ECS> ecs)
	{
		return CreateRef<RenderSystem2D>(config, ecs);
	}
	RenderSystem2D::RenderSystem2D(const RenderSystem2DConfig& config, ref<ECS> ecs) : m_ECS(ecs), m_Config(config)
	{
		Core::Application::UseRenderAPI(m_Config.renderAPI);

		// Load all shaders
		m_MainShader = config.mainShader;
		m_PostProcShader = config.postProcShader;
		if (!config.mainShader)     m_MainShader = Renderer::DefaultShaders::LoadMainShader();
		if (!config.postProcShader) m_PostProcShader = Renderer::DefaultShaders::LoadPostProcShader();
		m_CircleShader = Renderer::DefaultShaders::LoadCircleShader();

		m_CurrentCamera = nullptr;

		// Init frame buffers
		Renderer::FrameBufferConfig frameBufferConfig;
		frameBufferConfig.width = config.surface.scale.x;
		frameBufferConfig.height = config.surface.scale.y;
		frameBufferConfig.attachments = { { Renderer::AttachmentType::ColorAttachment } };
		m_FrameBuffer = Renderer::FrameBuffer::Create(frameBufferConfig);

		m_OutputBuffer = nullptr;
		if (config.surface.frame != nullptr) {
			frameBufferConfig.attachments = { { Renderer::AttachmentType::ColorAttachment, config.surface.frame } };
			m_OutputBuffer = Renderer::FrameBuffer::Create(frameBufferConfig);
		}

		// Create blank texture
		Renderer::TextureConfig blankTextureConfig;
		blankTextureConfig.width = 1;
		blankTextureConfig.height = 1;
		blankTextureConfig.data = static_cast<uint8_t*>(malloc(3));
		if (blankTextureConfig.data) memset(blankTextureConfig.data, 255, 3);
		m_BlankTexture = Renderer::Texture::Create(blankTextureConfig);
		free(blankTextureConfig.data);

		// Create quad vertex array
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

		// Config render api
		Core::Application::UseRenderAPI(m_Config.renderAPI);
		Renderer::RenderAPI::SetClearColor({ 0,0,0,1 });
		Renderer::RenderAPI::SetClearMask(Renderer::ColorBufferBit | Renderer::DepthBufferBit);
		Renderer::RenderAPI::SetDrawMode(Renderer::DrawMode::Triangles);
		Renderer::RenderAPI::SetViewport(config.surface.scale.x, config.surface.scale.y);
	}
	RenderSystem2D::~RenderSystem2D()
	{

	}

	void RenderSystem2D::Resize(i32 width, i32 height)
	{
		Renderer::RenderAPI::SetViewport(width, height);
		m_FrameBuffer->Resize(width, height);
		if (m_OutputBuffer != nullptr)  m_OutputBuffer->Resize(width, height);
		if (m_CurrentCamera != nullptr) m_CurrentCamera->Resize(width, height);
	}

	void RenderSystem2D::StartFrame(ref<Renderer::Camera> camera)
	{
		Core::Application::UseRenderAPI(m_Config.renderAPI);
		Renderer::RenderAPI::Clear();

		m_CurrentCamera = camera;

		// Bind and clear frame buffer
		m_FrameBuffer->Bind();
		Renderer::RenderAPI::Clear();
	}
	void RenderSystem2D::EndFrame()
	{
		s_Quad->Bind();
		
		// Bind the quad shader
		m_MainShader->Bind();
		m_MainShader->SetUniform("ViewProjection", (void*)&m_CurrentCamera->GetViewProjection());

		// Draw colored quads
		View<Transform, ColoredQuad> coloredQuadView(m_ECS);
		if (!coloredQuadView.Empty()) {
			m_BlankTexture->Bind();
			m_MainShader->SetUniform("TextureMap", (void*)&m_BlankTexture);
			coloredQuadView.ForEach([&](Transform& transform, ColoredQuad& quad) { ColoredQuadSystem(transform, quad); });
			m_BlankTexture->Unbind();
		}

		// Draw textured quads
		View<Transform, TexturedQuad> texturedQuadView(m_ECS);
		if (!texturedQuadView.Empty()) {
			texturedQuadView.ForEach([&](Transform& transform, TexturedQuad& quad) { TexturedQuadSystem(transform, quad); });
		}
		m_MainShader->Unbind();

		// Bind the circle shader
		m_CircleShader->Bind();
		m_CircleShader->SetUniform("ViewProjection", (void*)&m_CurrentCamera->GetViewProjection());

		// Draw colored circles
		View<Transform, ColoredCircle> coloredCircleView(m_ECS);
		if (!coloredCircleView.Empty()) {
			m_BlankTexture->Bind();
			m_CircleShader->SetUniform("TextureMap", (void*)&m_BlankTexture);
			coloredCircleView.ForEach([&](Transform& transform, ColoredCircle& circle) { ColoredCircleSystem(transform, circle); });
			m_BlankTexture->Unbind();
		}

		// Draw textured circles
		View<Transform, TexturedCircle> texturedCircleView(m_ECS);
		if (!texturedCircleView.Empty()) {
			texturedCircleView.ForEach([&](Transform& transform, TexturedCircle& quad) { TexturedCircleSystem(transform, quad); });
		}
		m_CircleShader->Unbind();

		m_FrameBuffer->Unbind();
		
		// If the surface passed to the renderer has a frame then render to that frame with the output buffer
		if (m_OutputBuffer != nullptr) {
			m_OutputBuffer->Bind();
			Renderer::RenderAPI::Clear();
		}

		// Run the frame through the post process shader
		m_PostProcShader->Bind();
		ref<Renderer::Texture> frame = m_FrameBuffer->GetColorAttachment(0);

		frame->Bind();
		m_PostProcShader->SetUniform("FrameTexture", (void*)&frame);

		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);

		frame->Unbind();
		m_PostProcShader->Unbind();

		if (m_OutputBuffer != nullptr) m_OutputBuffer->Unbind();
		s_Quad->Unbind();
	}

	void RenderSystem2D::ColoredQuadSystem(Transform& transform, ColoredQuad& quad)
	{
		// Create a model matrix
		Transform worldTransform = transform.ApplyParent(m_ECS);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, worldTransform.position);
		model = model * glm::mat4_cast(worldTransform.rotation);
		//model = glm::rotate(model, transform.rotation.z, { 0,0,1 });
		model = glm::scale(model, worldTransform.scale);

		glm::vec4 tint = { quad.tint, 1.0f };

		// Draw quad
		m_MainShader->SetUniform("Model", (void*)&model);
		m_MainShader->SetUniform("Tint", (void*)&tint);
		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
	}
	void RenderSystem2D::TexturedQuadSystem(Transform& transform, TexturedQuad& quad)
	{
		// Create a model matrix
		Transform worldTransform = transform.ApplyParent(m_ECS);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, worldTransform.position);
		model = model * glm::mat4_cast(worldTransform.rotation);
		//model = glm::rotate(model, transform.rotation.z, { 0,0,1 });
		model = glm::scale(model, worldTransform.scale);

		glm::vec4 tint = { quad.tint, 1.0f };

		// Draw quad
		quad.texture->Bind();
		m_MainShader->SetUniform("TextureMap", (void*)&quad.texture);
		m_MainShader->SetUniform("Model", (void*)&model);
		m_MainShader->SetUniform("Tint", (void*)&tint);
		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
		quad.texture->Unbind();
	}

	void RenderSystem2D::ColoredCircleSystem(Transform& transform, ColoredCircle& circle)
	{
		// Create a model matrix
		Transform worldTransform = transform.ApplyParent(m_ECS);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, worldTransform.position);
		model = glm::scale(model, { worldTransform.scale.x, worldTransform.scale.x, 1 });

		glm::vec4 tint = { circle.tint, 1.0f };

		// Draw circle
		m_CircleShader->SetUniform("Model", (void*)&model);
		m_CircleShader->SetUniform("Tint", (void*)&tint);
		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
	}
	void RenderSystem2D::TexturedCircleSystem(Transform& transform, TexturedCircle& circle)
	{
		// Create a model matrix
		Transform worldTransform = transform.ApplyParent(m_ECS);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, worldTransform.position);
		model = model * glm::mat4_cast(worldTransform.rotation);
		model = glm::scale(model, { worldTransform.scale.x, worldTransform.scale.x, 1 });

		glm::vec4 tint = { circle.tint, 1.0f };

		// Draw quad
		circle.texture->Bind();
		m_CircleShader->SetUniform("TextureMap", (void*)&circle.texture);
		m_CircleShader->SetUniform("Model", (void*)&model);
		m_CircleShader->SetUniform("Tint", (void*)&tint);
		Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
		circle.texture->Unbind();
	}
}