#pragma once
#include "Enigma/Core/Event/Event.h"
#include <sstream>

namespace Enigma {
	namespace Core {

		namespace EventType {
			const uint64_t WindowClose  = Hash("WindowClose");
			const uint64_t WindowResize = Hash("WindowResize");
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

			WindowResize(int width, int height)
				:m_Width(width), m_Height(height) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Window Resize ( " << m_Width << ", " << m_Height << " )";
				return ss.str();
			}

			int GetWidth() { return m_Width; }
			int GetHeight() { return m_Height; }

		private:
			int m_Width;
			int m_Height;
		};

	}
}