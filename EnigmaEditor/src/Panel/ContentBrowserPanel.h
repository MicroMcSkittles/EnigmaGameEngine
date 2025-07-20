#pragma once
#include <string>
#include <map>
#include <filesystem>
#include <Enigma/Renderer/Texture.h>

#include "Serialization/Image.h"
#include "Assets/TextureAsset.h"

namespace Enigma {
	namespace Editor {

		enum class FileType {
			None = 0,
			Directory,
			Image,
			Text,
			Source,
			Asset
		};
		struct File {
			std::filesystem::path path;
			std::string name;
			FileType type;
			Renderer::Texture*& image;

			File(std::filesystem::directory_entry entry, Renderer::Texture*& image);
			~File();

			inline static std::map<std::string, FileType> FileTypeMap = {
				{ ".txt", FileType::Text },
				
				{ ".json", FileType::Source },
				{ ".vert", FileType::Source },
				{ ".frag", FileType::Source },

				{ ".png", FileType::Asset },
				{ ".jpg", FileType::Asset },
				{ ".jpeg", FileType::Asset },
			};
		};
		struct Directory {
			std::filesystem::path path;

			std::vector<File*> files;

			std::string loading;
			ImageLoader imageLoader;

			~Directory() {
				for (File* file : files) delete file;
			}
		};

		class ContentBrowserPanel {
		public:

			ContentBrowserPanel();
			~ContentBrowserPanel();

			void ImGui();

		private:
			void DirectoryTreeNode(std::filesystem::path directory);
			
			void DirectoryView();
			void Icon(File* file);

			void ChangeCurrentPath(std::filesystem::path currentPath);

			void FileRightClickMenu();
			void FolderRightClickMenu();

			void FileDragDropSource(File* file);

		private:
			std::filesystem::path m_ProjectPath;

			Directory* m_CurrentDirectory;

			bool m_HoveredExists;
			std::filesystem::path m_Hovered;

			File* m_SelectedFile;

			int m_IconSize;
			int m_IconPadding;
		};

	}
}