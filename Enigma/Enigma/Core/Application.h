#pragma once

namespace Enigma {
	namespace Core {

		class Application {
		public:
			Application();
			~Application();

			void run();

		private:
			inline static Application* s_Instance;
		};

		Application* CreateApplication(int argc, char** argv);

	};
};