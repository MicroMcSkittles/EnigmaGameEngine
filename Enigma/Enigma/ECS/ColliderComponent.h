#pragma once
#include "ECS/Component.h"
#include "Physics/Collider.h"

namespace Enigma {
	namespace ECS {

		class Collider2D : public Component {
		public:
			COMP_DEF(Collider2D);
			Collider2D(Transform* transform) {
				m_Transform = transform;
				m_ID = CreateID(this);
			}

			Transform* GetTransform() { return m_Transform; }

			virtual Physics::Collider2D* CreateCollider() = 0;

		protected:
			Transform* m_Transform;
		};

		class CircleCollider : public Collider2D {
		public:
			CircleCollider(Transform* transform, float radius)
				: Collider2D(transform) 
			{
				m_Radius = radius;
			}

			virtual Physics::Collider2D* CreateCollider() override {
				return new Physics::CircleCollider(m_Transform->GetPosition(), m_Radius);
			}

			float& GetRadius() { return m_Radius; }

		private:
			float m_Radius;
		};

	}
}