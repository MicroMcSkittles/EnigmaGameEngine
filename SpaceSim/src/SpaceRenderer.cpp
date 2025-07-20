#include "SpaceRenderer.h"

#include <Enigma/Core/Process/Application.h>

void SpaceRenderer::Init(Engine::Surface* surface) {
	LOG_ASSERT(s_Data != nullptr, "Renderer instance already exists");
	s_Data = new Data();

	using namespace Renderer;

	s_Data->surface = surface;

	Core::Application::UseRenderAPI(API::OpenGL);

	ShaderConfig shaderConfig;
	shaderConfig.vertexPath = "assets/mainShader.vert";
	shaderConfig.pixelPath = "assets/mainShader.frag";
	s_Data->mainShader = Shader::Create(shaderConfig);
	shaderConfig.vertexPath = "assets/rasterShader.vert";
	shaderConfig.pixelPath = "assets/rasterShader.frag";
	s_Data->rasterShader = Shader::Create(shaderConfig);

	// Create quad vertex array
	s_Quad = VertexArray::Create();
	s_Quad->Bind();

	VertexBuffer* vertexBuffer = VertexBuffer::Create(s_VertexLayout, Usage::StaticDraw);
	vertexBuffer->SetData(&s_QuadVertices[0], s_QuadVertices.size() * sizeof(s_QuadVertices[0]));
	vertexBuffer->InitAttribs();
	s_Quad->AttachBuffer(vertexBuffer);

	IndexBuffer* indexBuffer = IndexBuffer::Create(DataType::UnsignedInt, Usage::StaticDraw);
	indexBuffer->SetData(&s_QuadIndices[0], s_QuadIndices.size() * sizeof(s_QuadIndices[0]));
	s_Quad->AttachBuffer(indexBuffer);

	s_Quad->Unbind();

	FrameBufferConfig rasterFrameConfig;
	rasterFrameConfig.width = s_Data->surface->scale.x;
	rasterFrameConfig.height = s_Data->surface->scale.y;

	TextureConfig rasterDepthBufferConfig;
	rasterDepthBufferConfig.dataType = DataType::UnsignedInt_24_8;
	rasterDepthBufferConfig.internalFormat = TexFormat::DEPTH24_STENCIL8;
	rasterDepthBufferConfig.format = TexFormat::DEPTH_STENCIL;
	rasterDepthBufferConfig.width = s_Data->surface->scale.x;
	rasterDepthBufferConfig.height = s_Data->surface->scale.y;
	rasterDepthBufferConfig.minFilter = TexFilterMode::Nearest;
	rasterDepthBufferConfig.magFilter = TexFilterMode::Nearest;
	s_Data->rasterDepthBuffer = Texture::Create(rasterDepthBufferConfig);

	TextureConfig rasterColorBufferConfig;
	rasterColorBufferConfig.width = s_Data->surface->scale.x;
	rasterColorBufferConfig.height = s_Data->surface->scale.y;
	s_Data->rasterColorBuffer = Texture::Create(rasterColorBufferConfig);
	
	rasterFrameConfig.attachments = { 
		{ AttachmentType::ColorAttachment, s_Data->rasterColorBuffer },
		{ AttachmentType::DepthAttachment, s_Data->rasterDepthBuffer }
	};
	s_Data->rasterFrameBuffer = FrameBuffer::Create(rasterFrameConfig);

	RenderAPI::SetClearColor({ 0,0,0,1 });
	RenderAPI::SetClearMask(ColorBufferBit | DepthBufferBit);
	RenderAPI::SetDrawMode(DrawMode::Triangles);
	RenderAPI::SetViewport(s_Data->surface->scale.x, s_Data->surface->scale.y);

	s_Data->sceneData = nullptr;

	std::vector<Sphere> spheres;
	PackSceneData(spheres);
}

void SpaceRenderer::ReloadShaders() {
	using namespace Renderer;
	delete s_Data->mainShader;
	ShaderConfig shaderConfig;
	shaderConfig.vertexPath = "assets/mainShader.vert";
	shaderConfig.pixelPath = "assets/mainShader.frag";
	s_Data->mainShader = Shader::Create(shaderConfig);
	shaderConfig.vertexPath = "assets/rasterShader.vert";
	shaderConfig.pixelPath = "assets/rasterShader.frag";
	s_Data->rasterShader = Shader::Create(shaderConfig);
}

void SpaceRenderer::StartFrame(Renderer::Camera* camera) {
	using namespace Renderer;
	s_Data->currentCamera = camera;

	RenderAPI::Clear();

	s_Data->rasterFrameBuffer->Bind();
	RenderAPI::Clear();
}

void SpaceRenderer::Submit(const Sphere& sphere)
{
	s_Data->spheres.push_back(sphere);
}

void SpaceRenderer::Submit(const Line& line)
{
	using namespace Renderer;
	VertexArray* vao = VertexArray::Create();
	vao->Bind();

	VertexBuffer* vertexBuffer = VertexBuffer::Create({ DataType::Float3 }, Usage::StaticDraw);
	float vertices[6] = {
		line.p1.x, line.p1.y, line.p1.z,
		line.p2.x, line.p2.y, line.p2.z
	};
	vertexBuffer->SetData(vertices, sizeof(vertices));
	vertexBuffer->InitAttribs();
	vao->AttachBuffer(vertexBuffer);

	RenderAPI::SetDrawMode(DrawMode::Lines);
	s_Data->rasterShader->Bind();
	s_Data->rasterShader->SetUniform("ViewProjection", (void*)&s_Data->currentCamera->GetViewProjection());
	s_Data->rasterShader->SetUniform("Color", (void*)&line.color);
	RenderAPI::DrawArrays(0, 2);
	s_Data->rasterShader->Unbind();

	RenderAPI::SetDrawMode(DrawMode::Triangles);

	vao->Unbind();
	delete vao;
}

void SpaceRenderer::Submit(const Grid& grid)
{
	using namespace Renderer;
	grid.vao->Bind();


	RenderAPI::SetDrawMode(DrawMode::Lines);
	s_Data->rasterShader->Bind();
	s_Data->rasterShader->SetUniform("ViewProjection", (void*)&s_Data->currentCamera->GetViewProjection());
	s_Data->rasterShader->SetUniform("Color", (void*)&grid.color);
	RenderAPI::DrawIndexed(grid.indices.size(), DataType::UnsignedInt, NULL);
	s_Data->rasterShader->Unbind();

	RenderAPI::SetDrawMode(DrawMode::Triangles);

	grid.vao->Unbind();
}

void SpaceRenderer::EndFrame() {
	using namespace Renderer;

	s_Data->rasterFrameBuffer->Unbind();

	PackSceneData(s_Data->spheres);
	s_Data->spheres.clear();

	s_Data->mainShader->Bind();
	s_Data->sceneData->Bind();
	s_Data->rasterColorBuffer->Bind();
	s_Data->rasterDepthBuffer->Bind();
	s_Data->mainShader->SetUniform("Resolution", (void*)&s_Data->surface->scale);
	s_Data->mainShader->SetUniform("CameraPosition", (void*)&s_Data->currentCamera->GetPosition());
	s_Data->mainShader->SetUniform("InvProjection", (void*)&s_Data->currentCamera->GetInvProjection());
	s_Data->mainShader->SetUniform("InvView", (void*)&s_Data->currentCamera->GetInvView());
	s_Data->mainShader->SetUniform("SceneData", (void*)s_Data->sceneData);
	s_Data->mainShader->SetUniform("RasterFrame", (void*)s_Data->rasterColorBuffer);
	s_Data->mainShader->SetUniform("RasterDepthMap", (void*)s_Data->rasterDepthBuffer);
	s_Quad->Bind();
	RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
	s_Quad->Unbind();
	s_Data->rasterColorBuffer->Unbind();
	s_Data->rasterDepthBuffer->Unbind();
	s_Data->sceneData->Unbind();
	s_Data->mainShader->Unbind();

}

void SpaceRenderer::Resize(int width, int height)
{
	s_Data->surface->Resize(width, height);
	s_Data->rasterFrameBuffer->Resize(width, height);
	s_Data->currentCamera->Resize(width, height);
	Renderer::RenderAPI::SetViewport(width, height);
}

void SpaceRenderer::PackSceneData(const std::vector<Sphere>& spheres) {

	std::vector<float> sceneData;
	sceneData.resize(1 + 7 * spheres.size());

	uint64_t pointer = 0;
	sceneData[pointer++] = spheres.size();

	for (const Sphere& s : spheres) {
		sceneData[pointer++] = s.position.x;
		sceneData[pointer++] = s.position.y;
		sceneData[pointer++] = s.position.z;

		sceneData[pointer++] = s.radius;

		sceneData[pointer++] = s.color.r;
		sceneData[pointer++] = s.color.g;
		sceneData[pointer++] = s.color.b;
	}

	Renderer::TextureConfig textureConfig;
	textureConfig.width = 1 + 7 * spheres.size();
	textureConfig.height = 1;

	textureConfig.dataType = Renderer::DataType::Float;
	textureConfig.format = Renderer::TexFormat::RED;
	textureConfig.internalFormat = Renderer::TexFormat::RED32F;

	textureConfig.minFilter = Renderer::TexFilterMode::Nearest;
	textureConfig.magFilter = Renderer::TexFilterMode::Nearest;

	textureConfig.data = sceneData.data();

	if (s_Data->sceneData) delete s_Data->sceneData;
	s_Data->sceneData = Renderer::Texture::Create(textureConfig);
}