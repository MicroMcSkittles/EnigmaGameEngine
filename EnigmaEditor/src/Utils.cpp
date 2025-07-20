#include "Utils.h"
#include <vector>
#include <string>

namespace Enigma {
	namespace Editor {

		std::filesystem::path CondensePath(const std::filesystem::path& path)
		{
			std::vector<std::string> splitPath;

			std::string pathStr = path.string();
			size_t lastOffset = 0;
			size_t offset = 0;
			while ((offset = pathStr.find_first_of('\\', lastOffset)) != std::string::npos)
			{
				std::string directory = std::string(pathStr.begin() + lastOffset, pathStr.begin() + offset + 1);
				lastOffset = offset + 1;

				if (directory == "..\\") {
					splitPath.pop_back();
					continue;
				}

				splitPath.push_back(directory);
			}

			std::string rslt;

			for (std::string& dir : splitPath) {
				rslt.append(dir);
			}

			return std::filesystem::path(rslt);
		}

	}
}