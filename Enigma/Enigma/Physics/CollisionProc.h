#pragma once
#include "Core/Process/SubProcess.h"

namespace Enigma {
	namespace Physics {

		class CollisionProc : public Core::SubProcess {
		public:
			virtual void StartUp() {}
			virtual void ShutDown() {}

			virtual void Update(float deltaTime);
		};

	}
}