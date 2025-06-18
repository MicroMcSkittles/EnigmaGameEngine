#pragma once
#include "Core/Process/SubProcess.h"

namespace Enigma {
	namespace Core {
		class ScriptProc : public SubProcess {
		public:

			virtual void Update(float deltaTime) override;

			virtual void ShutDown() override;
		};
	}
}