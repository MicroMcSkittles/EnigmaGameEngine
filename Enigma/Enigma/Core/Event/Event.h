#pragma once
#include "Enigma/Core/Core.h"
#include "Enigma/Core/Utilities/Utilities.h"
#include <functional>
#include <string>

namespace Enigma {
	namespace Core {

		enum EventCategory {
			EventNone = BIT(0),
			WindowEvent = BIT(1),
			InputEvent = BIT(2),
			MouseEvent = BIT(3),
			KeyboardEvent = BIT(4),
			Physics2DEvent = BIT(5),
			PhysicsEvent = BIT(6),
			LastEventCategoryBit = 6
		};
		
#define EVENT_TYPE(type) virtual uint64_t GetType() override { return EventType::type; } \
						static uint64_t StaticGetType() { return EventType::type; } \
						virtual std::string GetName() override { return #type; }

#define EVENT_CATEGORY(category) virtual uint32_t GetCategory() { return category; }

		class EventHandler;

		class Event {
		public:
			virtual uint64_t GetType() = 0;
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

			// To bind a function I recommend using a lamda in the following format
			// [&](Event& e)  { return func(e); }
			// you could use std::bind but it allocates extra memory for some reason
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