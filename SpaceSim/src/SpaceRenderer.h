#pragma once

#include <Enigma/Renderer/Shader.h>
#include <Enigma/Renderer/Camera.h>
#include <Enigma/Renderer/Buffer.h>
#include <Enigma/Renderer/VertexArray.h>
#include <Enigma/Renderer/RenderAPI.h>

#include "Simulation.h"

using namespace Enigma;

struct Sphere {
	glm::vec3 position;
	float radius;
	glm::vec3 color;
};
struct Line {
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 color;
};

class SpaceRenderer {
public:

	static void Init(Engine::Surface* surface);

	static void ReloadShaders();

	static void StartFrame(Renderer::Camera* camera);

	static void Submit(const Sphere& sphere);
	static void Submit(const Line& line);
	static void Submit(const Grid& grid);

	static void EndFrame();

	static void Resize(int width, int height);

private:

	static void PackSceneData(const std::vector<Sphere>& spheres);

private:
	struct Data {
		Engine::Surface* surface;

		Renderer::Shader* rasterShader;
		Renderer::Shader* mainShader;

		Renderer::Camera* currentCamera;

		std::vector<Sphere> spheres;
		Renderer::Texture* sceneData;

		Renderer::FrameBuffer* rasterFrameBuffer;
		Renderer::Texture* rasterDepthBuffer;
		Renderer::Texture* rasterColorBuffer;
	};
	inline static Data* s_Data;

private:
	struct Vertex {
		glm::vec3 position;
		glm::vec2 textureCoord;
	};

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