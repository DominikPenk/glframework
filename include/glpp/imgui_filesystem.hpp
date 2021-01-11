#pragma once

#include "glpp/imgui.hpp"
#include "glpp/texture.hpp"

#include <set>
#include <unordered_map>

#ifndef IMGUI_BOOKMARKS_FILE
#define IMGUI_BOOKMARKS_FILE std::string("imgui_fs.ini")
#endif

typedef int ImFileSystemFlags;

enum ImFileSystemFlags_ {
	ImFileSystemFlags_None                  = 0,
	ImFileSystemFlags_HideFiles             = 1,
	ImFileSystemFlags_FileSelect            = 1 << 1,
	ImFileSystemFlags_FolderSelect          = 1 << 2,

	ImFileSystemFlags_ShowHiddenEntries     = 1 << 3,
	ImFileSystemFlags_NoPreviews            = 1 << 4,

	ImFileSystemFlags_NoAcceptOnDoubleClick = 1 << 5
};


namespace ImGui {

	class FilePreview {
	public:
		virtual void selected(std::string path) = 0;
		virtual void draw() = 0;
	};

	class ImagePreview : public FilePreview {
	public:
		void selected(std::string path);
		void draw();
	protected:
		std::unique_ptr<gl::LargeTexture> mPreviewTexture;
		std::string mPath;
	};

	struct ImFilesystemDialoguePopup {
		ImFilesystemDialoguePopup();

		std::string currentSelected;
		std::string currentFolder;
		bool isOpen;

		std::string _currentFolderUserInput;
		std::string _newFolder;
		std::vector<std::string> _history;
		int _historyPos;
		bool _creatingNewFolder;
		std::shared_ptr<FilePreview> currentPreview;

		void setCurrentFolder(std::string folder);
		void setPreviewFunction(std::string file);
		void historyBack();
		void historyForward();

		static std::set<std::string> Bookmarks;
		static ImPool<ImFilesystemDialoguePopup> FileDialogues;
		static std::unordered_map<std::string, std::shared_ptr<FilePreview>> PreviewFunctions;
		static std::unordered_map<std::string, std::string> FileIcons;
		
		static bool _FirstCall;
		static std::vector<std::string> _Drives;
		static void _LoadBookmarks();
		static void AddBookmark(std::string path);
		static void RemoveBookmark(std::string path);
	};

	bool DirectoryTree(ImGuiID id, std::string& selected, const char* root = NULL, ImFileSystemFlags flags = ImFileSystemFlags_None, std::string ignoreIfStartsWith=".$");

	bool FilesystemDialogPopupModal(const char* label, std::string& selected, ImFileSystemFlags flags = ImFileSystemFlags_FileSelect);

	bool FileSystemPathInputEx(const char* label, std::string& path, size_t maxSize, const char* msg, ImFileSystemFlags flags);
	bool FilePathInput(const char* label, std::string& file, size_t maxSize, const char* msg = "Select a file");	// FIXME: Add extension filters
	bool FolderPathInput(const char* label, std::string& file, size_t maxSize, const char* msg = "Select a folder");
}