#include "Enigma/Engine/Physics/PhysicsEngine2D.h"

namespace Enigma::Engine::Physics {

	PhysicsEngine2D::PhysicsEngine2D(ECS::ECS* ecs, const PhysicsEngine2DConfig& config) : m_ECS(ecs), m_Config(config)
	{
		m_AccumulatedTime = 0.0f;
	}
	PhysicsEngine2D::~PhysicsEngine2D()
	{
	}

	void PhysicsEngine2D::Update(const DeltaTime& deltaTime)
	{
		ECS::View<ECS::Transform, RidgidBody2D> physicsView(m_ECS);
		// Run as many steps that can fit into deltaTime
		m_AccumulatedTime += deltaTime;
		while (m_AccumulatedTime >= m_Config.stepSize) {

			// Update every ridgid body in the scene
			physicsView.ForEach([&](ECS::Transform& transform, RidgidBody2D& ridgidBody) {
				Step(transform, ridgidBody);
			});

			m_AccumulatedTime -= m_Config.stepSize;
		}
	}
	void PhysicsEngine2D::Step(ECS::Transform& transform, RidgidBody2D& ridgidBody)
	{
		// TODO: figure out how quaturnions work
		// TODO: impl angular momentum

		// Calculate the rotation of the ridgid body
		glm::mat3 cross(0.0f);
		cross[0][1] = -ridgidBody.angularVelocity;
		cross[1][0] = ridgidBody.angularVelocity;
		ridgidBody.rotationMatrix += (cross * ridgidBody.rotationMatrix) * m_Config.stepSize;

		transform.rotation = glm::quat_cast(ridgidBody.rotationMatrix);
		//transform.rotation = glm::rotate(transform.rotation, ridgidBody.angularVelocity, { 0.0f,0.0f,1.0f });
		transform.position += glm::vec3(ridgidBody.linearVelocity * m_Config.stepSize, 0.0f);
	}

}