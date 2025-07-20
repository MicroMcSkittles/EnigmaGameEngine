#include "Panel/ContentBrowserPanel.h"
#include "Serialization/Image.h"
#include "EditorIcons.h"
#include "EditorEvent.h"
#include "Editor.h"

#include <imgui.h>
#include <Enigma/Core/Core.h>
#include <Enigma/ImGui/EnigmaWidgets.h>
#include <Enigma/Core/Process/Application.h>

namespace Enigma {
	namespace Editor {

		ContentBrowserPanel::ContentBrowserPanel()
		{
			// Load icons
			ImageConfig config;
			config.minFilter = Renderer::TexFilterMode::Nearest;
			config.magFilter = Renderer::TexFilterMode::Nearest;

			// Setup directories
			m_ProjectPath = Editor::GetActiveProject()->GetRootPath();
			//m_ProjectPath = "C:\\Users\\noahl\\Pictures";
			m_CurrentDirectory = nullptr;
			ChangeCurrentPath(m_ProjectPath);

			m_HoveredExists = false;

			m_IconSize = 128;
			m_IconPadding = 10;
		}
		ContentBrowserPanel::~ContentBrowserPanel()
		{
			if (m_CurrentDirectory) delete m_CurrentDirectory;
		}

		void ContentBrowserPanel::ImGui()
		{
			using namespace std::filesystem;

			ImGui::Begin("Content Browser");

			// Back button, doesn't let user go back past the project root
			path parentPath = m_CurrentDirectory->path.parent_path();
			if (m_CurrentDirectory->path != m_ProjectPath) {
				if (ImGui::Button("<-")) 
					ChangeCurrentPath(parentPath);
				ImGui::SameLine();
			}
			ImGui::Text(m_CurrentDirectory->path.string().c_str());

			// Directory Tree
			ImGuiChildFlags directoryTreeFlags = ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_Border;
			ImGui::BeginChild("CONTENT_BROWSER_DIRECTORY_TREE", { 150,0 }, directoryTreeFlags);
			DirectoryTreeNode(m_ProjectPath);
			ImGui::EndChild();

			ImGui::SameLine();

			// Directory View
			ImGuiChildFlags directoryViewFlags = ImGuiChildFlags_Border;
			ImGui::BeginChild("CONTENT_BROWSER_DIRECTORY_VIEW", { 0,0 }, directoryViewFlags);
			DirectoryView();
			ImGui::EndChild();

			ImGui::End();
		}

		void ContentBrowserPanel::DirectoryTreeNode(std::filesystem::path directory)
		{
			using namespace std::filesystem;

			if (directory != m_ProjectPath) {
				// Get directory name from path
				std::string directoryStr = directory.string();
				std::string directoryName = std::string(directoryStr.begin() + directoryStr.find_last_of('\\') + 1, directoryStr.end());
				
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				bool opened = ImGui::TreeNodeEx(directoryName.c_str(), flags);

				if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
					ChangeCurrentPath(directory);
				}

				if (!opened) return;
			}

			// Sort every directory based on if they have children
			std::vector<std::pair<directory_entry, bool>> sorted;
			directory_iterator dir_it(directory);
			for (directory_entry entry : dir_it) {
				if (!entry.is_directory()) continue;

				// See if directory has any child directories
				directory_iterator child_it(entry);
				bool hasChildrenDirectories = false;
				for (directory_entry child : child_it) {
					if (!child.is_directory()) continue;
					hasChildrenDirectories = true;
					break;
				}

				// Store in sorted
				if (hasChildrenDirectories) {
					sorted.insert(sorted.begin(), { entry, true });
					continue;
				}
				sorted.push_back({ entry, false });
			}
			for (auto& [entry, parent] : sorted) {
				// If directory has child directories then create a directory tree node
				// else make a selecable element
				if(parent) DirectoryTreeNode(entry);
				else {
					// Get entry directory name
					std::string entryStr = entry.path().string();
					std::string entryName = std::string(entryStr.begin() + entryStr.find_last_of('\\') + 1, entryStr.end());
					
					// Show selectable imgui element
					ImGuiSelectableFlags flags = ImGuiSelectableFlags_AllowDoubleClick;
					if (ImGui::Selectable(entryName.c_str(), false, flags) && ImGui::IsMouseDoubleClicked(0)) {
						m_CurrentDirectory->path = entry.path();
					}
				}
			}

			if (directory != m_ProjectPath) ImGui::TreePop();
		}

		void ContentBrowserPanel::DirectoryView()
		{
			using namespace std::filesystem;

			m_CurrentDirectory->imageLoader.IsLoading();

			int viewWidth = ImGui::GetContentRegionAvail().x;
			int cursor = m_IconSize;

			if (!m_HoveredExists) m_Hovered = "";
			m_HoveredExists = false;

			// Loop through every file in the current directory
			for (int i = 0; i < m_CurrentDirectory->files.size(); ++i) {
				File* file = m_CurrentDirectory->files[i];
				Icon(file);

				// Handle clicks
				if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
					if (file->type == FileType::Directory) ChangeCurrentPath(file->path);
					else if (file->type == FileType::Asset) {
						Asset* asset = Editor::GetActiveProject()->GetAssetHandler()->GetAsset(file->path.string());
						NewInspectorContext newContext(asset->GetInspectorContext());
						Core::Application::EventCallback(newContext);
					}
					else  {
						LOG_WARNING("Clicked entry ( %s )", file->path.string().c_str());
					}
				}
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
					ImGui::OpenPopup("CONTENT_BROWSER_PANEL_FILE_RIGHT_CLICK_MENU");
					m_SelectedFile = file;
				}

				// Move icon cursor
				cursor += m_IconSize + m_IconPadding;
				if (cursor > viewWidth) cursor = m_IconSize;
				else ImGui::SameLine();		
			}

			// Right click menu stuff
			if (ImGui::BeginPopup("CONTENT_BROWSER_PANEL_FILE_RIGHT_CLICK_MENU")) {
				FileRightClickMenu();
				ImGui::EndPopup();
			}
			else if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup("CONTENT_BROWSER_PANEL_FOLDER_RIGHT_CLICK_MENU");
			}
			if (ImGui::BeginPopup("CONTENT_BROWSER_PANEL_FOLDER_RIGHT_CLICK_MENU")) {
				FolderRightClickMenu();
				ImGui::EndPopup();
			}
		}

		void ContentBrowserPanel::Icon(File* file)
		{
			// flags
			bool hovered = (m_Hovered == file->path);

			ImGuiChildFlags iconFlags = ImGuiChildFlags_AutoResizeY;
			iconFlags |= (hovered) ? ImGuiChildFlags_Border : 0;
			ImGui::BeginChild(file->path.string().c_str(), { (float)m_IconSize,0 }, iconFlags);
			
			// Handle Drag/Drop stuff
			FileDragDropSource(file);

			if (file->type == FileType::Asset && file->image == nullptr) {
				Editor::GetActiveProject()->GetAssetHandler()->GetAsset(file->path.string())->GetAssetIcon();
			}

			// If there's no image than use the file icon
			Renderer::Texture* image = file->image;
			if (image == nullptr) image = Icons::File;

			ImGui::Image(image, {0,(float)(m_IconSize - (hovered ? (((float)m_IconSize) / 10.0f) : 0)) });
			ImGui::TextWrapped(file->name.c_str());

			ImGui::EndChild();

			// Handle hovering over file/directory
			if (ImGui::IsItemHovered()) {
				m_Hovered = file->path;
				m_HoveredExists = true;
			}

		}

		void ContentBrowserPanel::ChangeCurrentPath(std::filesystem::path currentPath)
		{
			using namespace std::filesystem;

			// Delete/Replace m_CurrentDirectory
			if (m_CurrentDirectory) delete m_CurrentDirectory;
			m_CurrentDirectory = new Directory();

			m_CurrentDirectory->path = currentPath;

			int directoryCount = 0; // Used for sorting folders
			// Loop through every file in the current path
			directory_iterator dir_it(m_CurrentDirectory->path);
			for (directory_entry entry : dir_it) {

				File* file;

				FileType type;
				if (entry.is_directory()) type = FileType::Directory;
				else if (File::FileTypeMap.count(entry.path().extension().string()))
					type = File::FileTypeMap[entry.path().extension().string()];
				else type = FileType::None;

				switch (type)
				{
				case FileType::Text:      file = new File(entry, Icons::Document); break;
				case FileType::Source:    file = new File(entry, Icons::Script); break;
				case FileType::Directory: {
					file = new File(entry, Icons::Folder); break;
					m_CurrentDirectory->files.insert(m_CurrentDirectory->files.begin() + directoryCount, file);
					directoryCount += 1;
				} continue;
				case FileType::Asset: {
					Asset* asset = Editor::GetActiveProject()->GetAssetHandler()->GetAsset(entry.path().string());
					asset->userCount += 1;
					file = new File(entry, asset->GetAssetIcon());
				} break;
				default: file = new File(entry, Icons::File);
				}
				m_CurrentDirectory->files.push_back(file);
			}
		}

		void ContentBrowserPanel::FileRightClickMenu()
		{
			ImGui::Text("W.I.P.");
			ImGui::MenuItem("Open");
			ImGui::MenuItem("Delete");
		}

		void ContentBrowserPanel::FolderRightClickMenu()
		{
			ImGui::Text("W.I.P.");
			ImGui::MenuItem("New Folder");
		}

		void ContentBrowserPanel::FileDragDropSource(File* file)
		{
			if (!ImGui::BeginDragDropSource()) return;

			ImGui::SetDragDropPayload("CONTENT_BROWSER_FILE_DRAG_SOURCE", (void*)file, sizeof(File));

			Renderer::Texture* image = file->image;
			if (image == nullptr) image = Icons::File;
			ImGui::Image(image, { 0, 128 });
			ImGui::TextWrapped(file->name.c_str());

			ImGui::EndDragDropSource();
		}

		File::File(std::filesystem::directory_entry entry, Renderer::Texture*& image) : image(image), path(entry.path())
		{
			if (entry.is_directory()) {
				std::string directoryStr = entry.path().string();
				name = std::string(directoryStr.begin() + directoryStr.find_last_of('\\') + 1, directoryStr.end());
				type = FileType::Directory;
				return;
			}

			name = path.filename().string();
			if (FileTypeMap.count(path.extension().string()))
				type = FileTypeMap[path.extension().string()];
			else
				type = FileType::None;
		}
		File::~File()
		{
			if (type == FileType::Asset) {
				Asset* asset = Editor::GetActiveProject()->GetAssetHandler()->GetAsset(path.string());
				asset->userCount -= 1;
				asset->Unload();
			}
		}
	}
}