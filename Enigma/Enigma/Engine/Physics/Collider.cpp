#include "Enigma/Engine/Physics/Collider.h"

namespace Enigma {
	namespace Engine {
		namespace Physics {

			bool Collider2D::Collide(Collider2D* collider)
			{
				switch (m_Type)
				{
				case Physics::Collider2DType::Circle: return CollideCircle(collider);
				case Physics::Collider2DType::Box: return CollideBox(collider);
				}
				return false;
			}

			bool CircleCollider::CollideCircle(Collider2D* collider)
			{
				CircleCollider* other = (CircleCollider*)collider;

				float dist = glm::distance(m_Position, other->GetPosition());
				float radiiSum = m_Radius + other->GetRadius();
				if (dist < radiiSum) {
					return true;
				}

				return false;
			}
			bool CircleCollider::CollideBox(Collider2D* collider)
			{
				LOG_ERROR("BoxCollider not impl yet");
				return false;
			}

			bool BoxCollider::CollideCircle(Collider2D* collider)
			{
				LOG_ERROR("BoxCollider not impl yet");
				return false;
			}
			bool BoxCollider::CollideBox(Collider2D* collider)
			{
				LOG_ERROR("BoxCollider not impl yet");
				return false;
			}
		}
	}
}