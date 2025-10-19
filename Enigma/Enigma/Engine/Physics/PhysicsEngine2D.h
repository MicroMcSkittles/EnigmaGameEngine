#pragma once
#include "Enigma/Engine/ECS/EntityComponentSystem.h"
#include "Enigma/Engine/ECS/Components.h"
#include "Enigma/Engine/Physics/PhysicsComponents.h"
#include "Enigma/Engine/DeltaTime.h"

namespace Enigma::Engine::Physics {

	// returns void takes in both entities that collided
	using CollisionCallback = std::function<void(ECS::EntityID, ECS::EntityID)>;

	struct PhysicsEngine2DConfig {
		float stepSize = 0.001f;
	};

	class PhysicsEngine2D {
	public:
		PhysicsEngine2D(ECS::ECS* ecs, const PhysicsEngine2DConfig& config = { });
		~PhysicsEngine2D();

		void Update(const DeltaTime& deltaTime);

	private:
		void Step(ECS::Transform& transform, RidgidBody2D& ridgidBody);

	private:
		ECS::ECS* m_ECS;
		PhysicsEngine2DConfig m_Config;
		Core::SparseSet<CollisionCallback> m_CollisionCallbacks;

		float m_AccumulatedTime;
	};

}