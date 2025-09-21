#pragma once
#include "Enigma/Core/Event/Event.h"
#include "Enigma/Core/System.h"
#include "Enigma/Engine/InputCodes.h"
#include <sstream>

namespace Enigma {
	namespace Core {

		namespace EventType {
			const uint64_t MouseMoved  = Hash("MouseMoved");
			const uint64_t MouseButton = Hash("MouseButton");
			const uint64_t MouseScroll = Hash("MouseScroll");
			const uint64_t Keyboard    = Hash("Keyboard");
		}

		class MouseMoved : public Event {
		public:
			EVENT_TYPE(MouseMoved);
			EVENT_CATEGORY(EventCategory::InputEvent | EventCategory::MouseEvent);

			MouseMoved(float x, float y)
				: m_X(x), m_Y(y) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Mouse Moved ( " << m_X << ", " << m_Y << " )";
				return ss.str();
			}

			float GetX() { return m_X; }
			float GetY() { return m_Y; }

		private:
			float m_X;
			float m_Y;
		};

		class MouseScroll : public Event {
		public:
			EVENT_TYPE(MouseScroll);
			EVENT_CATEGORY(EventCategory::InputEvent | EventCategory::MouseEvent);

			MouseScroll(float x, float y)
				: m_X(x), m_Y(y) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Mouse Scoll ( " << m_X << ", " << m_Y << " )";
				return ss.str();
			}

			float GetX() { return m_X; }
			float GetY() { return m_Y; }

		private:
			float m_X;
			float m_Y;
		};

		class MouseButton : public Event {
		public:
			EVENT_TYPE(MouseScroll);
			EVENT_CATEGORY(EventCategory::InputEvent |EventCategory::MouseEvent);

			MouseButton(int button, int action, int mods)
				: m_Button(button), m_Action(action), m_Mods(mods) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Mouse Button ( " << m_Button;
				ss << " ), Action ( " << System::GetActionName(m_Action);
				ss << " ), Mods " << System::GetModsName(m_Mods);
				return ss.str();
			}

			int GetButton() { return m_Button; }
			int GetAction() { return m_Action; }
			int GetMods() { return m_Mods; }

		private:
			int m_Button;
			int m_Action;
			int m_Mods;
		};

		class Keyboard : public Event {
		public:
			EVENT_TYPE(Keyboard);
			EVENT_CATEGORY(EventCategory::InputEvent | EventCategory::KeyboardEvent);

			Keyboard(int key, int scancode, int action, int mods)
				: m_Key(key), m_Scancode(scancode), m_Action(action), m_Mods(mods) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Keyboard, Key ( " << System::GetKeyName(m_Key);
				ss << " ), Scancode ( " << m_Scancode;
				ss << " ), Action ( " << System::GetActionName(m_Action);
				ss << " ), Mods " << System::GetModsName(m_Mods);
				return ss.str();
			}

			int GetKey() { return m_Key; }
			int GetAction() { return m_Action; }
			int GetMods() { return m_Mods; }

			bool IsShortcut(int key, int mods) {
				if (m_Action != Engine::KeyCode::KeyPress) return false;
				if (m_Mods != mods) return false;
				if (m_Key != key) return false;
				return true;
			}

		private:
			int m_Key;
			int m_Scancode;
			int m_Action;
			int m_Mods;
		};
	}
}
