#pragma once
#include "Core/Event/Event.h"
#include "ECS/Entity.h"

namespace Enigma {
	namespace Physics {
		class Collision2D : public Core::Event {
		public:
			EVENT_TYPE(Collision2D);
			EVENT_CATEGORY(Core::EventCategory::Physics2DEvent);

			Collision2D(ECS::Entity* self, ECS::Entity* other)
				: m_Self(self), m_Other(other) { }
			virtual std::string ToString() override {
				return "Collision Event";
			}

			ECS::Entity* GetSelf() { return m_Self; }
			ECS::Entity* GetOther() { return m_Other; }

		private:
			ECS::Entity* m_Self;
			ECS::Entity* m_Other;
		};
	}
}