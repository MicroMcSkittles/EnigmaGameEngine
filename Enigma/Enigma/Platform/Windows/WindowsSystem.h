#pragma once
#include "Core/System.h"

namespace Enigma {
	namespace Platform {

        class WindowsSystem : public Core::System {
        protected:
            virtual Core::Time GetTimeImpl() override;
            virtual float GetTimeMSImpl() override;

            virtual std::string GetOSNameImpl() override;
        };

	}
}