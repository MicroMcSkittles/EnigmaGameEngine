#include <Enigma/Enigma.h>
#include <Enigma/Core/Input.h>
#include <Enigma/Core/EntryPoint.h>

#include <Enigma/Renderer/RenderAPI.h>
#include <Enigma/Renderer/VertexArray.h>
#include <Enigma/Renderer/Shader.h>
#include <Enigma/Renderer/Texture.h>
#include <Enigma/Renderer/Camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Enigma;

class MainProc : public Core::SubProcess {
public:
	virtual void StartUp()override {
		Renderer::ShaderConfig config;
		config.vertexPath = "assets/shader.vert";
		config.pixelPath = "assets/shader.frag";
		m_Shader = Renderer::Shader::Create(config);

		float vertex_data[] = {
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,	1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f,	0.0f, 0.0f,
			-0.5f,  0.5f, 0.0f,	0.0f, 1.0f
		};
		unsigned int index_data[] = {
			0,1,3,
			1,2,3
		};

		m_Quad = Renderer::VertexArray::Create();
		m_Quad->Bind();

		Renderer::VertexBuffer* vbo = Renderer::VertexBuffer::Create(
			{ Renderer::DataType::Float3, Renderer::DataType::Float2 }, 
			Renderer::Usage::StaticDraw
		);
		vbo->SetData((void*)vertex_data, sizeof(vertex_data));
		vbo->Initialize();
		m_Quad->AttachBuffer(vbo);

		Renderer::IndexBuffer* ebo = Renderer::IndexBuffer::Create(Renderer::DataType::UnsignedInt, Renderer::Usage::StaticDraw);
		ebo->SetData(index_data, sizeof(index_data));
		ebo->Initialize();
		m_Quad->AttachBuffer(ebo);

		m_Quad->Unbind();

		m_Texture = Renderer::Texture::Create("assets/test.jpg");

		Renderer::Frustum frustum;
		frustum = Renderer::Frustum::ScreenFrustum();
		m_Camera = new Renderer::PerspectiveCamera(frustum, { 0,0,3 }, {0,0,-1});

		m_Rotation = 0;

		Renderer::RenderAPI::SetDrawMode(Renderer::DrawMode::Triangles);
		Renderer::RenderAPI::SetClearMask(Renderer::ColorBufferBit);
		Renderer::RenderAPI::SetClearColor({ 0,0,0,1 });

		LOG_MESSAGE("Using " + Core::System::GetOSName(), 2);
	}
	virtual void ShutDown() {
		LOG_MESSAGE("Main process shutting down :(", 2);
	}

	bool OnResize(Core::WindowResize& e) {
		Renderer::RenderAPI::SetViewport(e.GetWidth(), e.GetHeight());
		m_Camera->Resize(e.GetWidth(), e.GetHeight());
		return false;
	}
	virtual bool OnEvent(Core::Event& e) {
		Core::EventHandler handler(e);
		handler.Dispatch<Core::WindowResize>(CLASS_BIND_ARGS_1(MainProc::OnResize));
		return e.Handled();
	}

	virtual void Update(float deltaTime) override {
		m_Rotation += deltaTime * 50;
	}
	virtual void Render() override {
		Renderer::RenderAPI::Clear();

		m_Texture->Bind();

		glm::mat4 rot = glm::rotate(glm::mat4(1), glm::radians(m_Rotation), { 0,0,1 });

		m_Shader->SetUniform("tex", (void*)m_Texture);
		m_Shader->SetUniform("ViewProjection", (void*)&m_Camera->GetViewProjection());
		m_Shader->SetUniform("Model", (void*)&rot);

		m_Shader->Bind();
		m_Quad->Bind();
		Renderer::RenderAPI::DrawIndexed(m_Quad->GetIndexBuffer()->GetIndexCount(), m_Quad->GetIndexBuffer()->GetIndexType(), NULL);
		m_Quad->Unbind();
		m_Shader->Unbind();
		m_Texture->Unbind();
	}

private:
	Renderer::Shader* m_Shader;
	Renderer::VertexArray* m_Quad;
	Renderer::Texture* m_Texture;
	Renderer::Camera* m_Camera;

	float m_Rotation;
};

class App : public Core::Application {
public:
	App(Core::ApplicationConfig config, int argc, char** argv) : Application(config, argc, argv)
	{
		CreateSubProc<MainProc>();
	}
};

Enigma::Core::Application* Enigma::Core::CreateApplication(int argc, char** argv) {
	Core::WindowConfig windowConfig;
	windowConfig.title = "Example Application";
	windowConfig.width = 800;
	windowConfig.height = 600;
	windowConfig.resizable = true;
	windowConfig.vSync = true;

	Core::LoggerConfig loggerConfig;
	loggerConfig.flags = Core::LoggerFile | Core::LoggerFunction | Core::LoggerPriority | Core::LoggerTime;
	loggerConfig.priorityLevel = 5;
	
	ApplicationConfig config;
	config.rendererAPI = Renderer::API::OpenGL;
	config.windowConfig = windowConfig;
	config.loggerConfig = loggerConfig;
	return new App(config, argc, argv);
}