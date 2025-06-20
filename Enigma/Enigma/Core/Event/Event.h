#pragma once
#include "Core/Core.h"
#include <functional>
#include <string>

namespace Enigma {
	namespace Core {
		enum class EventType {
			None = 0,
			WindowClose, WindowResize,
			MouseMoved, MouseButton, MouseScroll,
			Keyboard,
			Collision2D, Collision,
		};

		enum EventCategory {
			EventNone = BIT(0),
			WindowEvent = BIT(1),
			InputEvent = BIT(2),
			MouseEvent = BIT(3),
			KeyboardEvent = BIT(4),
			Physics2DEvent = BIT(5),
			PhysicsEvent = BIT(6)
		};
		
#define EVENT_TYPE(type) virtual Core::EventType GetType() override { return Core::EventType::type; } \
						static Core::EventType StaticGetType() { return Core::EventType::type; } \
						virtual std::string GetName() override { return #type; }

#define EVENT_CATEGORY(category) virtual uint32_t GetCategory() { return category; }

		class EventHandler;

		class Event {
		public:
			virtual EventType GetType() = 0;
			virtual uint32_t GetCategory() = 0;
			virtual std::string GetName() = 0;
			virtual std::string ToString() { return GetName(); }

			void Handled(bool handled) { m_Handled = handled; }
			bool Handled() { return m_Handled; }

		private:
			friend EventHandler;
			bool m_Handled = false;
		};

		class EventHandler {
		public:
			EventHandler(Event& event) 
				: m_Event(event) { }

			template<typename T>
			void Dispatch(std::function<bool(T&)> func) {
				if (T::StaticGetType() == m_Event.GetType()) {
					m_Event.m_Handled = func(*(T*)&m_Event);
				}
			}
 
		private:
			Event& m_Event;
		};
	}
}