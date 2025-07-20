#pragma once

#include <Enigma/Engine/DeltaTime.h>
#include <Enigma/Core/IdHandler.h>
#include <Enigma/Renderer/VertexArray.h>

#include <glm/glm.hpp>

using namespace Enigma;

struct Planet {
	glm::vec3 startingPosition;
	glm::vec3 startingVelocity;
	double mass;
	float radius;
	glm::vec3 color;

	Core::ID id;
	glm::vec3 position;
	glm::vec3 velocity;

	bool operator ==(const Planet& other) const {
		return
			(position == other.position) &&
			(velocity == other.velocity) &&
			(mass == other.mass) &&
			(radius == other.radius) &&
			(color == other.color) &&
			(id == other.id);
	}
};

struct OrbitPath {
	std::vector<glm::vec3> points;
};

struct Grid {
	std::vector<glm::vec3> points;
	std::vector<uint32_t> indices;
	Renderer::VertexArray* vao;
	glm::vec3 color;
	void UpdateVAO();
	void DefaultGrid(int width, int height, float interval);
	~Grid();
};

class Simulation {
public:
	Simulation(double gravConstant);
	~Simulation();

	void Update(Engine::DeltaTime deltaTime);
	void Step(float t);

	void ImGui();

	OrbitPath GetOrbitPath(Core::ID planetID, Core::ID relative);
	Grid& GetGrid() { return m_Grid; }

	std::vector<Planet>& GetPlanets();
	Planet& GetPlanet(Core::ID id);
	Core::ID AddPlanet(const Planet& planet = {} );
	void RemovePlanet(Core::ID id);

private:
	glm::vec3 GetPlanetAffect(const Planet& planet, const Planet& other);



private:
	struct {
		int UPS;
		int lastUPS;
		float time;
	} m_Stats;

	double m_GravConstant;
	double m_SpeedOfLight;
	float m_Time;
	float m_UpdateRate;
	float m_Step;
	bool m_Running;
	int m_UpdatesPerSecond;
	Grid m_Grid;
	Core::IDHandler<Planet> m_Planets;
};