#include "Core/Process/Application.h"
#include "Core/Core.h"
#include "Core/System.h"
#include "Renderer/RenderAPI.h"

//TEMP delete later
#include "Core/Input.h"
#include <glad/glad.h>
#include <iostream>

namespace Enigma {
	namespace Core {
		void Application::Initialize(const ApplicationConfig& config)
		{
			if (s_Instance) {
				LOG_SOFT_ERROR("Application Instance already exists");
				return;
			}
			s_Instance = this;

			INIT_LOGGER(config.loggerConfig);

			m_Window = Window::Create(config.windowConfig, CLASS_BIND_ARGS_1(Application::OnEvent));

			Renderer::RenderAPI::Init(config.rendererAPI);

			m_IsRunning = true;
		}

		Application::Application(const ApplicationConfig& config) {
			Initialize(config);
		}
		Application::Application(const ApplicationConfig& config, int argc, char** argv)
		{
			m_Arguments = std::vector<std::string>(argv, argv + argc);

			Initialize(config);
		}
		Application::~Application()
		{
			// Destroy all processes
			for (auto& proc : m_SubProcStack.GetData()) {
				proc->ShutDown();
			}
		}

		void Application::OnEvent(Event& e)
		{
			EventHandler handler(e);
			handler.Dispatch<WindowClose>(CLASS_BIND_ARGS_1(Application::OnWindowClose));

			m_SubProcStack.OnEvent(e);
		}
		bool Application::OnWindowClose(WindowClose& e)
		{
			m_IsRunning = false;
			return false;
		}

		// TEMP: delete later ==============

		unsigned int program;
		unsigned int vao;

		void GL_INIT() {

			std::string vertexSource = System::ReadFileStr("ExampleApp/assets/shader.vert");
			std::string fragmentSource = System::ReadFileStr("ExampleApp/assets/shader.frag");

			unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
			const char* cVertexSource = vertexSource.c_str();
			glShaderSource(vertex, 1, &cVertexSource, NULL);
			glCompileShader(vertex);

			int success = 0;
			char info[512];
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if(!success) {
				glGetShaderInfoLog(vertex, 512, NULL, info);
				std::string infoLog = info;
				LOG_ERROR("Failed to compile shader\n" + infoLog);
			}

			unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
			const char* cFragmentSource = fragmentSource.c_str();
			glShaderSource(fragment, 1, &cFragmentSource, NULL);
			glCompileShader(fragment);

			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if(!success) {
				glGetShaderInfoLog(fragment, 512, NULL, info);
				std::string infoLog = info;
				LOG_ERROR("Failed to compile shader\n" + infoLog);
			}

			program = glCreateProgram();
			glAttachShader(program, vertex);
			glAttachShader(program, fragment);
			glLinkProgram(program);
			glDeleteShader(vertex);
			glDeleteShader(fragment);

			glGetProgramiv(program, GL_LINK_STATUS, &success);
			if(!success) {
				glGetProgramInfoLog(program, 512, NULL, info);
				std::string infoLog = info;
				LOG_ERROR("Failed to link program\n" + infoLog);
			}

			float vertex_data[] = {
				-0.5, -0.5, 0.0,
				 0.5, -0.5, 0.0,
				 0.0,  0.5, 0.0
			};
			unsigned int index_data[] = {
				0,1,2
			};

			glCreateVertexArrays(1, &vao);
			glBindVertexArray(vao);

			unsigned int vbo;
			glCreateBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			unsigned int ebo;
			glCreateBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);
			glBindVertexArray(0);

			glClearColor(0,0,0,1);
		}

		void GL_UPDATE() {
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(program);
			glBindVertexArray(vao);
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0);
			glUseProgram(0);
		}
		// =================================

		void Application::run()
		{
			GL_INIT();
			while (m_IsRunning) {

				float deltaTime = 0;

				GL_UPDATE();

				m_SubProcStack.Update(deltaTime);

				m_SubProcStack.Render();

				m_SubProcStack.ImGui();

				m_Window->Update();
			}
		}
	};
};