#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

#include <Enigma/Core/Input.h>
#include <Enigma/Core/InputCodes.h>
#include <Enigma/Core/Event/InputEvent.h>

#include <Enigma/ECS/Component.h>
#include <Enigma/ECS/Entity.h>
#include <Enigma/ECS/RenderComponent.h>
#include <Enigma/ECS/ColliderComponent.h>

#include <Enigma/Physics/CollisionEvent.h>

#include <iostream>

using namespace Enigma;

const glm::vec3 c_ShipScale = { 0.25f, 0.25f, 1.0f };
const glm::vec3 c_BulletScale = { 0.125f, 0.125f, 1.0f };
const float c_PlayerLayer = 0;
const float c_EnemyLayer = -1;
const float c_BackgroundLayer = -2;

const glm::vec2 c_PlayerTexture = { 1,1 };
const glm::vec2 c_EnemyTexture = { 0,0 };
const glm::vec2 c_PlayerBulletTexture = { 0,1 };
const glm::vec2 c_EnemyBulletTexture = { 0,0 };

static Renderer::TextureAtlas* s_ShipAtlas;
static Renderer::TextureAtlas* s_BulletAtlas;

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
				e->AddComponent<ECS::Render2D>(new ECS::Render2D({ 1,1,1 }, c_BackgroundLayer + m_ParalaxStrength, m_Texture));
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
		Renderer::Texture::Delete(m_Texture);
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

class BulletScript : public ECS::Script {
public:
	BulletScript(const glm::vec2& position, const glm::vec2& direction, bool fromPlayer)
		: Script(), m_StartPosition(position), m_Direction(direction), m_FromPlayer(fromPlayer) { }

	virtual void Start() {

		// Get bullet
		ECS::Entity* self = ECS::Entity::Get(GetParentID());

		// Set tag
		self->GetTag() = "BULLET";

		// Set transform
		m_Transform = self->GetComponent<ECS::Transform>();
		m_Transform->GetPosition() = { m_StartPosition, 0.0f };
		m_Transform->GetScale() = c_BulletScale;
		m_Transform->GetRotation().z = atan2(m_Direction.x, m_Direction.y);

		// Create Render2D component
		ECS::Render2D* renderComp = new ECS::Render2D({1,1,1}, c_EnemyLayer);
		if (m_FromPlayer) renderComp->SetTexture(s_BulletAtlas->GetTexture(c_PlayerBulletTexture));
		else renderComp->SetTexture(s_BulletAtlas->GetTexture(c_EnemyBulletTexture));
		self->AddComponent<ECS::Render2D>(renderComp);

		// Add collider
		self->AddComponent<ECS::CircleCollider>(new ECS::CircleCollider(m_Transform, c_BulletScale.x));

		// Init member variables
		m_Speed = 5.0f;
		m_LifeSpan = 5.0f;
		m_Time = 0.0f;

		m_Direction = { m_Direction.x, -m_Direction.y };
	}
	virtual void Update(float deltaTime) {
		// Check if bullet should be destroyed
		m_Time += deltaTime;
		if (m_Time >= m_LifeSpan) {
			ECS::Entity::Delete(ECS::Entity::Get(GetParentID()));
		}
		// Update position
		m_Transform->GetPosition() += glm::vec3(m_Direction* (deltaTime* m_Speed), 0);
	}
	virtual void Shutdown() {

	}

	bool IsFromPlayer() { return m_FromPlayer; }

private:
	ECS::Transform* m_Transform;

	glm::vec2 m_StartPosition;
	glm::vec2 m_Direction;
	float m_Speed;
	float m_LifeSpan;
	float m_Time;

	bool m_FromPlayer;
};

// Player
class PlayerScript : public ECS::Script {
public:
	PlayerScript(ECS::Entity* camera) 
		: Script(), m_Camera(camera) { }

	virtual void Start() override {
		// Get player entity
		ECS::Entity* self = ECS::Entity::Get(GetParentID());

		// Init transform
		m_Transform = self->GetComponent<ECS::Transform>();
		m_Transform->GetScale() = c_ShipScale;

		// Add collider
		self->AddComponent<ECS::CircleCollider>(new ECS::CircleCollider(m_Transform, c_ShipScale.x));
	}
	virtual void Update(float deltaTime) override {

		// Make sure player is alive
		if (m_Health <= 0.0f) {
			LOG_ERROR("you ded lol");
		}

		// Update m_TintTime
		m_TintTime -= 5.0f * deltaTime;
		if (m_TintTime < 0.0f) m_TintTime = 0.0f;

		// Update the tint of the enemy based on m_TintTime
		float tintScale = 1.0f - (m_TintTime / 2.0f);
		// Tints the enemy red
		glm::vec3 tint = { 1.0f, tintScale, tintScale };
		ECS::Render2D* renderComp = ECS::Entity::Get(GetParentID())->GetComponent<ECS::Render2D>();
		renderComp->GetTint() = tint;

		// When I wrote the camera controlls only me and god knew how it worked, now only god knows
		// it smoothly moves the camera to the player

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

		// If the player moved then normalize the move vector and update the player position
		if (move.x != 0 || move.y != 0) {
			move = glm::normalize(move);
			move *= deltaTime * m_Speed;
			m_Transform->GetPosition() += glm::vec3(move, 0.0f);
			m_IsMoving = true; // idk what this is even for
		}
		else m_IsMoving = false;

		// Rotation controlls
		glm::vec2 mousePos = Core::Input::GetMousePosition();
		glm::vec2 playerScreenPos = m_Camera->GetComponent<ECS::Camera>()->ScreenPosition(m_Transform->GetPosition());
		
		// Find the direction between the players position and the mouse position
		glm::vec2 mouseDir = mousePos - playerScreenPos;
		if (glm::length(mouseDir) == 0) mouseDir = { 1, 0 };
		else mouseDir = glm::normalize(mouseDir);

		// Rotation the player toward the mouse
		m_Direction = mouseDir;
		m_Transform->GetRotation().z = atan2(mouseDir.x, mouseDir.y);
	}
	virtual void Shutdown() override {
		
	}

	bool OnCollision(Physics::Collision2D& e) {
		if (e.GetOther()->GetTag() != "BULLET") return false;
		BulletScript* bullet = (BulletScript*)e.GetOther()->GetComponent<ECS::Script>();
		if (bullet->IsFromPlayer()) return false;

		ECS::Entity::Delete(e.GetOther());
		m_TintTime = 2.0f;
		m_Health -= 1;

		return false;
	}
	virtual void OnEvent(Core::Event& e) override {
		Core::EventHandler handler(e);
		handler.Dispatch<Physics::Collision2D>(CLASS_BIND_ARGS_1(PlayerScript::OnCollision));
	}

	glm::vec2 GetDirection() {
		return m_Direction;
	}

private:
	ECS::Transform* m_Transform;
	ECS::Entity* m_Camera;
	
	glm::vec2 m_Direction;

	float m_Health = 10.0f;
	float m_TintTime = 0.0f;

	float m_Speed = 4.5f;
	float m_CameraLeadIn = 3.0f;
	float m_CameraLeadOut = 2.5f;

	bool m_IsMoving = false;
};

class EnemyScript : public ECS::Script {
public:
	EnemyScript(ECS::Transform* playerTransform, int& enemyCount)
	: Script(), m_PlayerTransform(playerTransform), m_EnemyCount(enemyCount) { }

	virtual void Start() override {
		m_EnemyCount += 1;

		// Get enemy entity
		ECS::Entity* self = ECS::Entity::Get(GetParentID());

		// Get transform
		m_Transform = self->GetComponent<ECS::Transform>();

		// Init Render2D component
		ECS::Render2D* renderComp = self->GetComponent<ECS::Render2D>();
		renderComp->SetTexture(s_ShipAtlas->GetTexture(c_EnemyTexture));

		// Add collider
		self->AddComponent<ECS::CircleCollider>(new ECS::CircleCollider(m_Transform, c_ShipScale.x));

		// Generate random stats for the enemy to use ( speed, turnspeed, ect )
		RandomStats();
	}
	virtual void Update(float deltaTime) override {

		// Make sure enemy is alive
		if (m_Health <= 0.0f) {
			ECS::Entity::Delete(ECS::Entity::Get(GetParentID()));
			return;
		}

		// Update m_TintTime
		m_TintTime -= 5.0f * deltaTime;
		if (m_TintTime < 0.0f) m_TintTime = 0.0f;

		// Update the tint of the enemy based on m_TintTime
		float tintScale = 1.0f - (m_TintTime / 2.0f);
		// Tints the enemy red
		glm::vec3 tint = { 1.0f, tintScale, tintScale };
		ECS::Render2D* renderComp = ECS::Entity::Get(GetParentID())->GetComponent<ECS::Render2D>();
		renderComp->GetTint() = tint;

		// Find the direction vector from the enemy to the player
		glm::vec2 directionToPlayer = m_PlayerTransform->GetPosition() - m_Transform->GetPosition();
		if (glm::length(directionToPlayer) == 0) directionToPlayer = { 1,0 };
		else directionToPlayer = glm::normalize(directionToPlayer);

		// Check if enemy should shoot the player
		bool shoot = (glm::length(directionToPlayer + m_Direction) >= m_FireRange);
		if (shoot) {
			if (m_Time <= 0.0f) {
				m_Time = m_FireTime;
			}
			else shoot = false;
		}
		m_Time -= deltaTime;
		if (m_Time < 0) m_Time = 0;

		// Shoot the player if allowed
		if (shoot) {
			ECS::Entity* bullet = ECS::Entity::Create();
			glm::vec2 direction = { directionToPlayer.x, -directionToPlayer.y };
			bullet->AddComponent<ECS::Script>(new BulletScript(m_Transform->GetPosition(), direction, false));
		}

		// Rotate the enemy if its direction points to far from the directionToPlayer vector
		glm::vec2 newDirection = { };
		if (glm::length(directionToPlayer + m_Direction) <= 0.2) {
			// Choose a random direction to move the enemy
			if (rand() % 2) newDirection = glm::vec2(directionToPlayer.y, -directionToPlayer.x);
			else newDirection = glm::vec2(directionToPlayer.y, directionToPlayer.x);
		}
		else {
			// I don't remember what this is for and I don't feel like figuring it out
			newDirection = deltaTime * m_TurnSpeed * directionToPlayer + m_Direction;
		}

		// Rotate the enemy based on the new direction
		float length = glm::length(newDirection);
		m_Direction = glm::normalize(newDirection);
		m_Transform->GetRotation().z = atan2(m_Direction.x, -m_Direction.y);

		length = glm::clamp(length, 0.1f, 3.0f);

		// Update position
		m_Transform->GetPosition() += glm::vec3(m_Direction * deltaTime * m_Speed * length, 0.0f);
	}
	virtual void Shutdown() override {
		m_EnemyCount -= 1;
	}

	bool OnCollision(Physics::Collision2D& e) {
		if (e.GetOther()->GetTag() != "BULLET") return false;
		BulletScript* bullet = (BulletScript*)e.GetOther()->GetComponent<ECS::Script>();
		if (!bullet->IsFromPlayer()) return false;

		ECS::Entity::Delete(e.GetOther());
		m_TintTime = 2.0f;
		m_Health -= 1;

		return false;
	}
	virtual void OnEvent(Core::Event& e) override {
		Core::EventHandler handler(e);
		handler.Dispatch<Physics::Collision2D>(CLASS_BIND_ARGS_1(EnemyScript::OnCollision));
	}

private:
	void RandomStats() {
		m_Speed = 1.0f + (rand() % 50) / 25;
		m_TurnSpeed = 2.0f + (rand() % 50) / 50;
		m_FireTime = 0.5f + (rand() % 6) / 2;
		m_Health = 1 + (rand() % 2);

		m_FireRange = glm::radians(20.0f);
		m_Time = 6.0f;

		m_TintTime = 0.0f;
	}

private:
	ECS::Transform* m_PlayerTransform;
	ECS::Transform* m_Transform;

	glm::vec2 m_Direction;

	int& m_EnemyCount;

	int m_Type;

	float m_Health;

	float m_Speed;
	float m_TurnSpeed;

	float m_FireRange;
	float m_FireTime;
	float m_Time;
	float m_TintTime;
};

// Spawns enemies
class EnemyManagerScript : public ECS::Script {
public:
	EnemyManagerScript(ECS::Transform* playerTransform)
		: Script(), m_PlayerTransform(playerTransform) { }

	virtual void Start() override {
		m_EnemyCount = 0;

		m_SpawnRadius = 5.0f;
		m_SpawnRate = 3.0f;
		m_Time = m_SpawnRate;
	}
	virtual void Update(float deltaTime) override {
		if (m_Time <= 0.0f) {
			glm::vec2 dir = {
			(rand() % 1000) / 1000.0f,
			(rand() % 1000) / 1000.0f,
			};
			dir = glm::normalize(dir);

			ECS::Entity* enemy = ECS::Entity::Create();

			ECS::Transform* transform = enemy->GetComponent<ECS::Transform>();
			transform->GetScale() = c_ShipScale;
			transform->GetPosition() = { glm::vec2(m_PlayerTransform->GetPosition()) + (dir *  m_SpawnRadius), 0.0f };

			enemy->AddComponent<ECS::Render2D>(new ECS::Render2D({ 1,1,1 }, c_EnemyLayer));
			enemy->AddComponent<ECS::Script>(new EnemyScript(m_PlayerTransform, m_EnemyCount));
			
			m_Time = m_SpawnRate;
		}

		m_Time -= deltaTime;
		if (m_Time < 0.0f) m_Time = 0.0f;
	}
	virtual void Shutdown() override {

	}

private:
	ECS::Transform* m_PlayerTransform;
	int m_EnemyCount;

	Renderer::TextureAtlas* m_ShipAtlas;

	float m_SpawnRadius;
	float m_SpawnRate;
	float m_Time;
};

class MainProc : public Core::SubProcess {
public:
	void Init() {
		// Setup pre process shader
		Renderer::ShaderConfig shaderConfig;
		shaderConfig.vertexPath = "ExampleApp/assets/postProc.vert";
		shaderConfig.pixelPath = "ExampleApp/assets/postProc.frag";
		m_PreProc = Renderer::Shader::Create(shaderConfig);

		// Create ship and bullet texture atlas
		Renderer::TextureConfig textureConfig;
		textureConfig.minFilter = Renderer::TexFilterMode::Nearest;
		textureConfig.magFilter = Renderer::TexFilterMode::Nearest;
		textureConfig.preProcShader = m_PreProc;
		s_ShipAtlas = new Renderer::TextureAtlas("ExampleApp/assets/textures/spaceships.png", { 5,2 }, textureConfig);
		s_BulletAtlas = new Renderer::TextureAtlas("ExampleApp/assets/textures/projectiles.png", { 3,2 }, textureConfig);

		// Create camera
		m_Camera = ECS::Entity::Create();
		Renderer::ViewBox view = Renderer::ViewBox::ScreenViewBox();
		m_Camera->AddComponent<ECS::Camera>(new ECS::OrthographicCamera(view, 2.0f, true));

		// Create player
		m_Player = ECS::Entity::Create();
		m_Player->AddComponent<ECS::Render2D>(new ECS::Render2D({ 1,1,1 }, c_PlayerLayer, s_ShipAtlas->GetTexture(c_PlayerTexture)));
		m_Player->AddComponent<ECS::Script>(new PlayerScript(m_Camera));
		auto playerTransform = m_Player->GetComponent<ECS::Transform>();

		// Create background manager
		m_Background = ECS::Entity::Create();
		m_Background->AddComponent<ECS::Script>(new BackgroudManagerScript(playerTransform));

		// Create enemy manager
		m_EnemyManager = ECS::Entity::Create();
		m_EnemyManager->AddComponent<ECS::Script>(new EnemyManagerScript(playerTransform));
	}

	virtual void StartUp() {
		unsigned int seed = (unsigned int)(Core::System::GetTimeMS() * 100);
		srand(seed);

		Init();
	}

	bool OnClick(Core::MouseButton& e) {
		// Fire a bullet when the player presses the left mouse button
		if (e.GetButton() == MOUSE_BUTTON_LEFT && e.GetAction() == PRESS) {
			ECS::Transform* playerPos = m_Player->GetComponent<ECS::Transform>();
			glm::vec2 dir = ((PlayerScript*)m_Player->GetComponent<ECS::Script>())->GetDirection();
			ECS::Entity* bullet = ECS::Entity::Create();
			bullet->AddComponent<ECS::Script>(new BulletScript(playerPos->GetPosition(), dir, true));
		}
		return false;
	}
	virtual bool OnEvent(Core::Event& e) override {
		Core::EventHandler handler(e);
		handler.Dispatch<Core::MouseButton>(CLASS_BIND_ARGS_1(MainProc::OnClick));
		return false;
	}

private:
	ECS::Entity* m_Player;
	ECS::Entity* m_Camera;
	ECS::Entity* m_Background;
	ECS::Entity* m_EnemyManager;

	Renderer::Shader* m_PreProc;
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
	windowConfig.vSync = false;

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