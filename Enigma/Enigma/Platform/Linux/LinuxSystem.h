#include "Core/System.h"

namespace Enigma {
    namespace Platform {
        
        class LinuxSystem : public Core::System {
        protected:
            virtual Core::Time GetTimeImpl() override;
            virtual float GetTimeMSImpl() override;

            virtual std::string GetOSNameImpl() override;
        };

    }
}