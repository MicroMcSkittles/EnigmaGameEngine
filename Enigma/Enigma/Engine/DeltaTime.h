#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Core/System.h"

namespace Enigma {
	namespace Engine {

		class DeltaTime {
		public:
			DeltaTime() {
				m_DeltaTime = 0;
				m_LastFrame = Core::System::GetTimeMS();
			}
			DeltaTime(f32 deltaTime) {
				m_DeltaTime = deltaTime;
				m_LastFrame = Core::System::GetTimeMS();
			}

			void Calculate() {
				f32 currentTime = Core::System::GetTimeMS();
				m_DeltaTime = currentTime - m_LastFrame;
				m_LastFrame = currentTime;
			}

			operator float() { return m_DeltaTime; }
			operator float() const { return m_DeltaTime; }

		private:
			f32 m_DeltaTime;
			f32 m_LastFrame;
		};

	}
}