#pragma once
#include "Core/IdHandler.h"
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace Physics {

		enum class Collider2DType {
			Circle,
			Box
		};

		class Collider2D {
		public:
			Collider2D(Collider2DType type) {
				m_ID = s_IDHandler.Create(this);
				m_Type = type;
			}
			~Collider2D() {
				s_IDHandler.Delete(m_ID);
			}

			bool Collide(Collider2D* collider);

		protected:
			virtual bool CollideCircle(Collider2D* collider) = 0;
			virtual bool CollideBox(Collider2D* collider) = 0;

		private:
			Collider2DType m_Type;
			Core::ID m_ID;

		private:
			inline static Core::IDHandler<Collider2D> s_IDHandler;
		};

		class CircleCollider : public Collider2D {
		public:
			CircleCollider(const glm::vec2& position, float radius)
				: Collider2D(Collider2DType::Circle), m_Position(position), m_Radius(radius) { }

			glm::vec2& GetPosition() { return m_Position; }
			float& GetRadius() { return m_Radius; }

		protected:
			virtual bool CollideCircle(Collider2D* collider) override;
			virtual bool CollideBox(Collider2D* collider) override;

		private:
			glm::vec2 m_Position;
			float m_Radius;
		};

		class BoxCollider : public Collider2D {
		public:
			BoxCollider() : Collider2D(Collider2DType::Box) { LOG_ERROR("BoxCollider not impl yet"); }

		protected:
			virtual bool CollideCircle(Collider2D* collider) override;
			virtual bool CollideBox(Collider2D* collider) override;
		};
	}
}