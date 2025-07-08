#include "Serialization/Project.h"

namespace Enigma {
	namespace Editor {

		Project::Project(const std::string& filename)
		{
			m_ProjectPath = filename;
			JSON::JSON::LoadFile(m_ProjectPath, &m_ProjectData);
		}

		Project::~Project()
		{
		}

		void Project::Save()
		{
		}

	}
}