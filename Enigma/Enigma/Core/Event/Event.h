#pragma once
#include "Enigma/Core/Core.h"
#include "Enigma/Core/Utilities/Utilities.h"

#include <functional>

namespace Enigma::Core {

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
		
#define EVENT_TYPE(type) virtual u64 GetType() override { return EventType::type; } \
						static u64 StaticGetType() { return EventType::type; } \
						virtual std::string GetName() override { return #type; }

#define EVENT_CATEGORY(category) virtual uint32_t GetCategory() override { return category; }

	class EventHandler;

	class Event {
	public:
		virtual u64 GetType() = 0;
		virtual u32 GetCategory() = 0;
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

		// If the event is of type T, then runs the passed function
		template<typename T>
		void Dispatch(std::function<bool(T&)> func) {
			if (T::StaticGetType() == m_Event.GetType()) {
				m_Event.m_Handled = func(*static_cast<T*>(&m_Event));
			}
		}
 
	private:
		Event& m_Event;
	};
	
}
