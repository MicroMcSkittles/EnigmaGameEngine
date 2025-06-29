#pragma once
#include "Core/System.h"

namespace Enigma {
	namespace Platform {

        class WindowsSystem : public Core::System {
        protected:
            virtual void InitImpl() override;
            virtual void ShutdownImpl() override;

            virtual Core::Time GetTimeImpl() override;
            virtual float GetTimeMSImpl() override;

            virtual std::string GetOSNameImpl() override;

            virtual std::string GetKeyNameImpl(int key) override;
            virtual std::string GetButtonNameImpl(int button) override;
            virtual std::string GetActionNameImpl(int action) override;
            virtual std::string GetModsNameImpl(int action) override;
        };

	}
}