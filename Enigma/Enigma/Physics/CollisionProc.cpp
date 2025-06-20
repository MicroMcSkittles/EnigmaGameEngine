#include "Physics/CollisionProc.h"

#include "Physics/CollisionEvent.h"
#include "Physics/Collider.h"

#include "ECS/ColliderComponent.h"
#include "ECS/Entity.h"

namespace Enigma {
	namespace Physics {
		void CollisionProc::Update(float deltaTime)
		{
			auto& colliders = ECS::Collider2D::GetList();

			for (auto& self : colliders) {
				for (auto& other : colliders) {
					if (self == other) continue;

					if (!ECS::Entity::IsValid(self->GetParentID())) continue;
					if (!ECS::Entity::IsValid(other->GetParentID())) continue;

					Physics::Collider2D* selfCollider = self->CreateCollider();
					Physics::Collider2D* otherCollider = other->CreateCollider();

					if (selfCollider->Collide(otherCollider)) {
						ECS::Entity* selfEntity = ECS::Entity::Get(self->GetParentID());

						if (!selfEntity->HasComponent<ECS::Script>()) continue;
						ECS::Script* script = selfEntity->GetComponent<ECS::Script>();
						ECS::Entity* otherEntity = ECS::Entity::Get(other->GetParentID());

						Collision2D collisionEvent(selfEntity, otherEntity);
						script->OnEvent(collisionEvent);
					}

					delete selfCollider;
					delete otherCollider;
				}
			}
		}
	}
}