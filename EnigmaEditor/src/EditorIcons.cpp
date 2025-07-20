#include "EditorIcons.h"
#include "Serialization/Image.h"

#include <Enigma/Core/Process/Application.h>

namespace Enigma {
	namespace Editor {
		void Icons::Load()
		{
			ImageConfig config;
			Script    = ImageLoader::Load(Icons::s_ScriptData,    sizeof(Icons::s_ScriptData),    config);
			Folder    = ImageLoader::Load(Icons::s_FolderData,    sizeof(Icons::s_FolderData),    config);
			File      = ImageLoader::Load(Icons::s_FileData,      sizeof(Icons::s_FileData),      config);
			Document  = ImageLoader::Load(Icons::s_DocumentData,  sizeof(Icons::s_DocumentData),  config);
			NoTexture = ImageLoader::Load(Icons::s_NoTextureData, sizeof(Icons::s_NoTextureData), config);
		}
	}
}