#pragma once
#include "Core/System.h"

namespace Enigma {
	namespace Engine {

		class DeltaTime {
		public:
			DeltaTime() {
				m_DeltaTime = 0;
				m_LastFrame = Core::System::GetTimeMS();
			}

			void Calculate() {
				float currentTime = Core::System::GetTimeMS();
				m_DeltaTime = currentTime - m_LastFrame;
				m_LastFrame = currentTime;
			}

			operator float() {
				return m_DeltaTime;
			}

		private:
			float m_DeltaTime;
			float m_LastFrame;
		};

	}
}