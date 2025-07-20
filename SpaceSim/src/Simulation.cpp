#include <Simulation.h>

#include <imgui.h>

Simulation::Simulation(double gravConstant)
{
	m_GravConstant = gravConstant;
	//m_UpdateRate = 0.025f;
	m_UpdatesPerSecond = 120;
	m_UpdateRate = 1.0f / m_UpdatesPerSecond;
	//m_UpdateRate = 1;
	m_Time = 0.0f;
	m_SpeedOfLight = 4;
	//m_SpeedOfLight = 0.5;
	m_Stats.time = 0;
	m_Stats.UPS = 0;
	m_Stats.lastUPS = 0;
	m_Running = true;

	m_Step = 0.02f;

	m_Grid.DefaultGrid(125, 125, 1);
}
Simulation::~Simulation()
{
	m_Planets.Clear();
}

void Simulation::Update(Engine::DeltaTime deltaTime)
{
	if (m_Stats.time >= 1) {
		m_Stats.time = 0;
		m_Stats.lastUPS = m_Stats.UPS;
		m_Stats.UPS = 0;
	}
	m_Stats.time += deltaTime;
	if (m_Time < m_UpdateRate) {
		m_Time += deltaTime;
		return;
	}
	m_Time = 0;
	Step(m_Step);
	m_Stats.UPS += 1;
	
}

void Simulation::Step(float t)
{
	float totalMass = 0;
	float comY = 0;
	for (Planet& planet : m_Planets.GetData()) {
		if (m_Running) {
			for (Planet& other : m_Planets.GetData()) {
				planet.velocity += GetPlanetAffect(planet, other);
			}
			planet.position += planet.velocity * t;
		}
		comY += planet.mass * planet.position.y;
		totalMass += planet.mass;
	}
	if (totalMass > 0) comY /= totalMass;

	float originalMaxY = -3.4028235e+38;
	for (int i = 0; i < m_Grid.points.size(); ++i) {
		originalMaxY = std::max(originalMaxY, m_Grid.points[i].y);
	}

	float verticalShift = comY -originalMaxY;

	for(glm::vec3& point : m_Grid.points) {

		glm::vec3 totalDisplacement(0.0f);

		for (Planet& planet : m_Planets.GetData()) {
			glm::vec3 direction = planet.position - point;
			float distance = glm::length(direction);
			float distanceM = distance * 1000;
			float rs = (2 * m_GravConstant * planet.mass) / (m_SpeedOfLight * m_SpeedOfLight);
			float dz = 2 * sqrt(rs * (distance - rs));
			totalDisplacement.y += dz * 2.0f;
		}
		point.y = totalDisplacement.y + -abs(verticalShift);
	}
	m_Grid.UpdateVAO();
}

void Simulation::ImGui()
{
	ImGui::Begin("Simulation");

	ImGui::Checkbox("Running", &m_Running);
	ImGui::Text("Updates Per Second %i", m_Stats.lastUPS);
	if (ImGui::InputInt("Target UPS", &m_UpdatesPerSecond)) {
		m_UpdateRate = 1.0f / (m_UpdatesPerSecond * 2);
	}
	ImGui::InputFloat("Step", &m_Step);
	ImGui::InputDouble("Gravitational Constant", &m_GravConstant);
	ImGui::InputDouble("Speed Of Light", &m_SpeedOfLight);

	if (ImGui::Button("Reset")) {
		for (Planet& planet : m_Planets.GetData()) {
			planet.position = planet.startingPosition;
			planet.velocity = planet.startingVelocity;
		}
		delete m_Grid.vao;
		m_Grid.points.clear();
		m_Grid.indices.clear();
		m_Grid.DefaultGrid(125, 125, 1);
	}

	ImGui::End();
}


OrbitPath Simulation::GetOrbitPath(Core::ID planetID, Core::ID relative)
{
	OrbitPath path;

	Simulation* simulation = new Simulation(m_GravConstant);
	simulation->m_Planets = m_Planets;

	for (int i = 0; i < 2000; ++i) {
		simulation->Step(0.02f);
		if (i % 50 == 1) {
			glm::vec3 point = simulation->GetPlanet(planetID).position;
			//glm::vec3 relativePoint = simulation->GetPlanet(relative).position;
			//path.points.push_back(point - relativePoint);
			path.points.push_back(point);
		}
	}

	delete simulation;
	return path;
}

glm::vec3 Simulation::GetPlanetAffect(const Planet& planet, const Planet& other)
{
	if (planet == other) return glm::vec3(0,0,0);

	float distance = glm::distance(planet.position, other.position);
	glm::vec3 direction = glm::normalize(other.position - planet.position);
	float gForce = m_GravConstant * ((planet.mass * other.mass) / (distance * distance));
	float acceleration = gForce / planet.mass;

	return direction * acceleration;
}


std::vector<Planet>& Simulation::GetPlanets()
{
	return m_Planets.GetData();
}
Planet& Simulation::GetPlanet(Core::ID id)
{
	LOG_ASSERT(!m_Planets.IsValid(id), "Failed to get planet, ID is invalid ( %s )", ((std::string)id).c_str());
	return m_Planets.Get(id);
}
Core::ID Simulation::AddPlanet(const Planet& planet)
{
	Core::ID id = m_Planets.Create(planet);
	m_Planets.Get(id).id = id;
	m_Planets.Get(id).position = planet.startingPosition;
	m_Planets.Get(id).velocity = planet.startingVelocity;
	return id;
}
void Simulation::RemovePlanet(Core::ID id)
{
	LOG_ASSERT(!m_Planets.IsValid(id), "Failed to remove planet, ID is invalid ( %s )", ((std::string)id).c_str());
	m_Planets.Delete(id);
}

void Grid::UpdateVAO()
{
	vao->GetVertexBuffer()->SetSubData(&points[0], points.size() * sizeof(points[0]), 0);
}

void Grid::DefaultGrid(int width, int height, float interval)
{
	float startX = ((float)width * interval) / -2.0f;
	float startZ = ((float)height * interval) / -2.0f;

	float currentX = startX;
	float currentZ = startZ;

	for (int z = 0; z < height; ++z) {
		for (int x = 0; x < width; ++x) {
			
			if (x != width - 1) {
				indices.push_back(points.size());
				indices.push_back(points.size() + 1);
			}
			if (z != height - 1) {
				indices.push_back(points.size());
				indices.push_back(points.size() + width);
			}
			points.push_back({ currentX, 0, currentZ });
			currentX += interval;
		}
		currentX = startX;
		currentZ += interval;
	}

	using namespace Renderer;

	vao = VertexArray::Create();
	vao->Bind();

	VertexBuffer* vertexBuffer = VertexBuffer::Create({ Renderer::DataType::Float3 }, Usage::StaticDraw);
	vertexBuffer->SetData(&points[0], points.size() * sizeof(points[0]));
	vertexBuffer->InitAttribs();
	vao->AttachBuffer(vertexBuffer);

	IndexBuffer* indexBuffer = IndexBuffer::Create(DataType::UnsignedInt, Usage::StaticDraw);
	indexBuffer->SetData(&indices[0], indices.size() * sizeof(indices[0]));
	vao->AttachBuffer(indexBuffer);

	vao->Unbind();

	color = { 0.25,0.25,0.25 };
}

Grid::~Grid()
{
	delete vao;
}
