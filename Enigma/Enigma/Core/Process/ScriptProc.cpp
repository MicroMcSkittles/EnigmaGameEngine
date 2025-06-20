#include "Core/Process/ScriptProc.h"

#include "ECS/Component.h"
#include "ECS/Entity.h"

namespace Enigma {
	namespace Core {
		void ScriptProc::Update(float deltaTime)
		{
			auto& list = ECS::Script::GetList();
			for (int i = 0; i < list.size(); ++i) {
				auto script = list[i];
				if (!ECS::Script::IsValid(script->GetID())) continue;
				if (!script->Started()) {
					script->Start();
					script->Started() = true;
				}

				script->Update(deltaTime);
			}
		}
		void ScriptProc::ShutDown()
		{
			for (auto& script : ECS::Script::GetList()) {
				script->Shutdown();
			}
			ECS::Script::GetHandler().Clear();
		}
	}
}