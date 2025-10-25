#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Engine/ECS/EntityComponentSystem.h"
#include "Enigma/Engine/ECS/Components.h"
#include "Enigma/Engine/Physics/PhysicsComponents.h"
#include "Enigma/Engine/DeltaTime.h"

namespace Enigma::Engine::Physics {

	// returns void takes in both entities that collided
	using CollisionCallback = std::function<void(ECS::EntityID, ECS::EntityID)>;

	struct PhysicsEngine2DConfig {
		f32 stepSize = 0.001f;
	};

	class PhysicsEngine2D {
	public:
		static ref<PhysicsEngine2D> Create(ref<ECS::ECS> ecs, const PhysicsEngine2DConfig& config = { });
		PhysicsEngine2D(ref<ECS::ECS> ecs, const PhysicsEngine2DConfig& config);
		~PhysicsEngine2D();

		void Update(const DeltaTime& deltaTime);

	private:
		void Step(ECS::Transform& transform, RidgidBody2D& ridgidBody);

	private:
		ref<ECS::ECS> m_ECS;
		PhysicsEngine2DConfig m_Config;
		Core::SparseSet<CollisionCallback> m_CollisionCallbacks;

		f32 m_AccumulatedTime;
	};

}