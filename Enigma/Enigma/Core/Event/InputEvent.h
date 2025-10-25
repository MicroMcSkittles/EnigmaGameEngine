#pragma once
#include "Enigma/Core/Event/Event.h"
#include "Enigma/Core/System.h"
#include "Enigma/Engine/InputCodes.h"
#include <sstream>

namespace Enigma {
	namespace Core {

		namespace EventType {
			const u64 MouseMoved  = Hash("MouseMoved");
			const u64 MouseButton = Hash("MouseButton");
			const u64 MouseScroll = Hash("MouseScroll");
			const u64 Keyboard    = Hash("Keyboard");
		}

		class MouseMoved : public Event {
		public:
			EVENT_TYPE(MouseMoved);
			EVENT_CATEGORY(EventCategory::InputEvent | EventCategory::MouseEvent);

			MouseMoved(f32 x, f32 y)
				: m_X(x), m_Y(y) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Mouse Moved ( " << m_X << ", " << m_Y << " )";
				return ss.str();
			}

			float GetX() { return m_X; }
			float GetY() { return m_Y; }

		private:
			f32 m_X;
			f32 m_Y;
		};

		class MouseScroll : public Event {
		public:
			EVENT_TYPE(MouseScroll);
			EVENT_CATEGORY(EventCategory::InputEvent | EventCategory::MouseEvent);

			MouseScroll(f32 x, f32 y)
				: m_X(x), m_Y(y) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Mouse Scoll ( " << m_X << ", " << m_Y << " )";
				return ss.str();
			}

			f32 GetX() { return m_X; }
			f32 GetY() { return m_Y; }

		private:
			f32 m_X;
			f32 m_Y;
		};

		class MouseButton : public Event {
		public:
			EVENT_TYPE(MouseScroll);
			EVENT_CATEGORY(EventCategory::InputEvent |EventCategory::MouseEvent);

			MouseButton(i32 button, i32 action, i32 mods)
				: m_Button(button), m_Action(action), m_Mods(mods) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Mouse Button ( " << m_Button;
				ss << " ), Action ( " << System::GetActionName(m_Action);
				ss << " ), Mods " << System::GetModsName(m_Mods);
				return ss.str();
			}

			i32 GetButton() { return m_Button; }
			i32 GetAction() { return m_Action; }
			i32 GetMods() { return m_Mods; }

		private:
			i32 m_Button;
			i32 m_Action;
			i32 m_Mods;
		};

		class Keyboard : public Event {
		public:
			EVENT_TYPE(Keyboard);
			EVENT_CATEGORY(EventCategory::InputEvent | EventCategory::KeyboardEvent);

			Keyboard(i32 key, i32 scancode, i32 action, i32 mods)
				: m_Key(key), m_Scancode(scancode), m_Action(action), m_Mods(mods) { }

			virtual std::string ToString() override {
				std::stringstream ss;
				ss << "Keyboard, Key ( " << System::GetKeyName(m_Key);
				ss << " ), Scancode ( " << m_Scancode;
				ss << " ), Action ( " << System::GetActionName(m_Action);
				ss << " ), Mods " << System::GetModsName(m_Mods);
				return ss.str();
			}

			i32 GetKey() { return m_Key; }
			i32 GetAction() { return m_Action; }
			i32 GetMods() { return m_Mods; }

			bool IsShortcut(i32 key, i32 mods) {
				if (m_Action != Engine::KeyCode::KeyPress) return false;
				if (m_Mods != mods) return false;
				if (m_Key != key) return false;
				return true;
			}

		private:
			i32 m_Key;
			i32 m_Scancode;
			i32 m_Action;
			i32 m_Mods;
		};
	}
}
