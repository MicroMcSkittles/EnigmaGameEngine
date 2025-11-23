#pragma once
#include "Enigma/Core/Event/Event.h"

namespace Enigma::Core {

	namespace EventType {
		const u64 WindowClose  = Hash("WindowClose");
		const u64 WindowResize = Hash("WindowResize");
	}

	class WindowClose : public Event {
	public:
		EVENT_TYPE(WindowClose);
		EVENT_CATEGORY(EventCategory::WindowEvent);

		WindowClose() { }
		virtual std::string ToString() override {
			return "Window Close";
		}
	};

	class WindowResize : public Event {
	public:
		EVENT_TYPE(WindowResize);
		EVENT_CATEGORY(EventCategory::WindowEvent);

		WindowResize(i32 width, i32 height)
			:m_Width(width), m_Height(height) { }

		virtual std::string ToString() override {
			std::stringstream ss;
			ss << "Window Resize ( " << m_Width << ", " << m_Height << " )";
			return ss.str();
		}

		i32 GetWidth() { return m_Width; }
		i32 GetHeight() { return m_Height; }

	private:
		i32 m_Width;
		i32 m_Height;
	};

}