#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

#include <Enigma/Core/Input.h>
#include <Enigma/Core/InputCodes.h>

#include <Enigma/ECS/Component.h>
#include <Enigma/ECS/Entity.h>
#include <Enigma/ECS/RenderComponent.h>

#include <iostream>

using namespace Enigma;

// Background stuff
class BackgroudScript : public ECS::Script {
public:
	BackgroudScript(const std::string& texturePath, float paralaxStrength, ECS::Transform* playerTransform)
		: Script(), m_TexturePath(texturePath), m_ParalaxStrength(paralaxStrength), m_PlayerTransform(playerTransform) { }

	virtual void Start() override {
		m_Transform = ECS::Entity::Get(GetParentID())->GetComponent<ECS::Transform>();
		
		Renderer::TextureConfig config;
		config.minFilter = Renderer::TexFilterMode::Nearest;
		config.magFilter = Renderer::TexFilterMode::Nearest;
		m_Texture = Renderer::Texture::Create(m_TexturePath, config);

		for (int x = -1; x <= 1; ++x) {
			for (int y = -1; y <= 1; ++y) {
				ECS::Entity* e = ECS::Entity::Create();
				e->AddComponent<ECS::Render2D>(new ECS::Render2D({ 1,1,1 }, m_ParalaxStrength - 1, m_Texture));
				auto transform = e->GetComponent<ECS::Transform>();
				transform->GetScale() = { m_TileSize / 2.0f, m_TileSize / 2.0f, 1 };
				transform->GetPosition() = { glm::vec2(x,y) * m_TileSize, 0 };

				m_Tiles.push_back(e);
			}
		}
	}
	virtual void Update(float deltaTime) override {

		// Get the current camera's transform
		ECS::Camera* camera = ECS::Camera::GetActiveCamera();
		ECS::Entity* cameraEntity = ECS::Entity::Get(camera->GetParentID());
		ECS::Transform* camaraTransform = cameraEntity->GetComponent<ECS::Transform>();

		glm::vec2 paralaxDist = camaraTransform->GetPosition();
		paralaxDist *= -m_ParalaxStrength;

		for (int x = -1; x <= 1; ++x) {
			for (int y = -1; y <= 1; ++y) {
				int tileIndex = (x + 1) + 3 * (y + 1);
				ECS::Entity* tile = m_Tiles[(uint64_t)tileIndex];

				glm::vec3 playerPos = m_PlayerTransform->GetPosition();
				glm::vec2 pos;
				pos.x = x + int((playerPos.x - paralaxDist.x) / m_TileSize);
				pos.y = y + int((playerPos.y - paralaxDist.y) / m_TileSize);
				pos *= m_TileSize;

				tile->GetComponent<ECS::Transform>()->GetPosition() = { pos + paralaxDist, 0 };
			}
		}
	}
	virtual void Shutdown() override {
		for (int i = 0; i < m_Tiles.size(); ++i) {
			ECS::Entity::Delete(m_Tiles[i]);
		}
		delete m_Texture;
	}

private:
	ECS::Transform* m_PlayerTransform;
	ECS::Transform* m_Transform;

	std::string m_TexturePath;
	Renderer::Texture* m_Texture;

	std::vector<ECS::Entity*> m_Tiles;

	float m_TileSize = 12;
	float m_ParalaxStrength = 0;
};
class BackgroudManagerScript : public ECS::Script {
public:
	BackgroudManagerScript(ECS::Transform* playerTransform)
		: Script(), m_PlayerTransform(playerTransform) { }

	virtual void Start() override {
		m_Dust = ECS::Entity::Create();
		m_Dust->AddComponent<ECS::Script>(new BackgroudScript("ExampleApp/assets/textures/SpaceBackgroundDust.png", 0, m_PlayerTransform));
		
		m_Nebulae = ECS::Entity::Create();
		m_Nebulae->AddComponent<ECS::Script>(new BackgroudScript("ExampleApp/assets/textures/SpaceBackgroundNebulae.png", 0.03, m_PlayerTransform));
		
		m_Stars = ECS::Entity::Create();
		m_Stars->AddComponent<ECS::Script>(new BackgroudScript("ExampleApp/assets/textures/SpaceBackgroundStars.png", 0.05, m_PlayerTransform));
		
		m_Planets = ECS::Entity::Create();
		m_Planets->AddComponent<ECS::Script>(new BackgroudScript("ExampleApp/assets/textures/SpaceBackgroundPlanets.png", 0.07, m_PlayerTransform));
	}
	virtual void Update(float deltaTime) override {

	}
	virtual void Shutdown() override {
		ECS::Entity::Delete(m_Dust);
		ECS::Entity::Delete(m_Nebulae);
		ECS::Entity::Delete(m_Stars);
		ECS::Entity::Delete(m_Planets);
	}

private:
	ECS::Transform* m_PlayerTransform;

	ECS::Entity* m_Dust;
	ECS::Entity* m_Nebulae;
	ECS::Entity* m_Stars;
	ECS::Entity* m_Planets;
};

// Player
class PlayerScript : public ECS::Script {
public:
	PlayerScript(ECS::Entity* camera) : Script(), m_Camera(camera) { }

	virtual void Start() override {
		m_Transform = ECS::Entity::Get(GetParentID())->GetComponent<ECS::Transform>();
		m_Transform->GetScale() = { 0.25,0.25,1 };

		auto render2DComp = ECS::Entity::Get(GetParentID())->GetComponent<ECS::Render2D>();
		Renderer::TextureConfig config;
		m_Texture = Renderer::Texture::Create("ExampleApp/assets/test.jpg", config);
		render2DComp->SetTexture(m_Texture);
	}
	virtual void Update(float deltaTime) override {

		// Camera Movement controlls
		auto cameraTransform = m_Camera->GetComponent<ECS::Transform>();
		float dist = glm::distance(m_Transform->GetPosition(), cameraTransform->GetPosition());
		if (dist >= 0.4) {
			glm::vec2 dir = m_Transform->GetPosition() - cameraTransform->GetPosition();
			dir = glm::normalize(dir);
			cameraTransform->GetPosition() += glm::vec3(dir * (deltaTime * m_CameraLeadIn * dist), 0);
		}
		else if (dist >= 0.02 && !m_IsMoving) {
			glm::vec2 dir = m_Transform->GetPosition() - cameraTransform->GetPosition();
			dir = glm::normalize(dir);
			cameraTransform->GetPosition() += glm::vec3(dir* (deltaTime * (m_CameraLeadOut * dist)), 0);
		}

		// Movement controlls
		glm::vec2 move = { 0,0 };
		if (Core::Input::IsKeyPressed(KEY_W)) move.y = 1;
		if (Core::Input::IsKeyPressed(KEY_S)) move.y = -1;
		if (Core::Input::IsKeyPressed(KEY_A)) move.x = -1;
		if (Core::Input::IsKeyPressed(KEY_D)) move.x = 1;

		if (move.x != 0 || move.y != 0) {
			move = glm::normalize(move);
			move *= deltaTime * m_Speed;
			m_Transform->GetPosition() += glm::vec3(move, 0.0f);
			m_IsMoving = true;
		}
		else m_IsMoving = false;

		// Rotation controlls
		glm::vec2 mousePos = Core::Input::GetMousePosition();
		glm::vec2 playerScreenPos = m_Camera->GetComponent<ECS::Camera>()->ScreenPosition(m_Transform->GetPosition());
		glm::vec2 mouseDir = mousePos - playerScreenPos;
		if (glm::length(mouseDir) == 0) mouseDir = { 1, 0 };
		else mouseDir = glm::normalize(mouseDir);
		m_Direction = mouseDir;
		m_Transform->GetRotation().z = atan2(-mouseDir.x, -mouseDir.y);
	}
	virtual void Shutdown() override {
		delete m_Texture;
	}

private:
	ECS::Transform* m_Transform;
	ECS::Entity* m_Camera;

	Renderer::Texture* m_Texture;

	glm::vec2 m_Direction;

	float m_Speed = 4.5f;
	float m_CameraLeadIn = 3.0f;
	float m_CameraLeadOut = 2.5f;

	bool m_IsMoving = false;
};

class MainProc : public Core::SubProcess {
public:
	virtual void StartUp() {

		// Create camera
		m_Camera = ECS::Entity::Create();
		Renderer::ViewBox view = Renderer::ViewBox::ScreenViewBox();
		m_Camera->AddComponent<ECS::Camera>(new ECS::OrthographicCamera(view, 2.0f, true));

		// Create player
		m_Player = ECS::Entity::Create();
		m_Player->AddComponent<ECS::Render2D>(new ECS::Render2D());
		m_Player->AddComponent<ECS::Script>(new PlayerScript(m_Camera));

		// Create background manager
		m_Background = ECS::Entity::Create();
		m_Background->AddComponent<ECS::Script>(new BackgroudManagerScript(m_Player->GetComponent<ECS::Transform>()));
	}

private:
	ECS::Entity* m_Player;
	ECS::Entity* m_Camera;
	ECS::Entity* m_Background;
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
	windowConfig.title = "Space Game";
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

	config.useRenderProc = true;

	return new App(config, argc, argv);
}