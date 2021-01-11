#include "glpp/imgui_filesystem.hpp"

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
#include <windows.h>
#undef min
#undef max
#endif

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <GLFW/glfw3.h>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

bool ImGui::ImFilesystemDialoguePopup::_FirstCall = true;
std::set<std::string> ImGui::ImFilesystemDialoguePopup::Bookmarks = std::set<std::string>();
std::vector<std::string> ImGui::ImFilesystemDialoguePopup::_Drives = std::vector<std::string>();
ImPool<ImGui::ImFilesystemDialoguePopup> ImGui::ImFilesystemDialoguePopup::FileDialogues = ImPool<ImGui::ImFilesystemDialoguePopup>();
std::unordered_map<std::string, std::shared_ptr<ImGui::FilePreview>> ImGui::ImFilesystemDialoguePopup::PreviewFunctions;
std::unordered_map<std::string, std::string> ImGui::ImFilesystemDialoguePopup::FileIcons = {
	{ ".png", ICON_FA_FILE_IMAGE },
	{ ".jpg", ICON_FA_FILE_IMAGE },
	{ ".jpeg", ICON_FA_FILE_IMAGE },
	{ ".bpm", ICON_FA_FILE_IMAGE },
	{ ".tiff", ICON_FA_FILE_IMAGE },
	{ ".tif", ICON_FA_FILE_IMAGE },
	{ ".exr", ICON_FA_FILE_IMAGE },
	{ ".ppm", ICON_FA_FILE_IMAGE },
	{ ".raw", ICON_FA_FILE_IMAGE },
	{ ".exe", ICON_FA_COG },
	{ ".ply", ICON_FA_DRAW_POLYGON },
	{ ".obj", ICON_FA_DRAW_POLYGON },
	{ ".txt", ICON_FA_FILE_ALT },
	{ ".pdf", ICON_FA_FILE_PDF },
	{ ".ppt", ICON_FA_FILE_POWERPOINT },
	{ ".pptx", ICON_FA_FILE_POWERPOINT },
	{ ".cxx", ICON_FA_FILE_CODE },
	{ ".c", ICON_FA_FILE_CODE },
	{ ".h", ICON_FA_FILE_CODE },
	{ ".hpp", ICON_FA_FILE_CODE },
	{ ".html", ICON_FA_FILE_CODE },
	{ ".py", ICON_FA_FILE_CODE },
	{ ".js", ICON_FA_FILE_CODE },
	{ ".cpp", ICON_FA_FILE_CODE },
	{ ".mp4", ICON_FA_FILE_VIDEO },
	{ ".avi", ICON_FA_FILE_VIDEO },
	{ ".webm", ICON_FA_FILE_VIDEO },
	{ ".mkv", ICON_FA_FILE_VIDEO },
	{ ".flv", ICON_FA_FILE_VIDEO },
	{ ".ogg", ICON_FA_FILE_VIDEO },
	{ ".ogv", ICON_FA_FILE_VIDEO },
	{ ".gif", ICON_FA_FILE_VIDEO },
	{ ".mov", ICON_FA_FILE_VIDEO },
	{ ".qt", ICON_FA_FILE_VIDEO },
	{ ".mpg", ICON_FA_FILE_VIDEO },
	{ ".zip", ICON_FA_FILE_ARCHIVE },
	{ ".7z", ICON_FA_FILE_ARCHIVE },
	{ ".mp3", ICON_FA_FILE_AUDIO },
	{ ".aac", ICON_FA_FILE_AUDIO },
	{ ".csv", ICON_FA_FILE_CSV },

};

#pragma region Utility functions

std::vector<std::string> getFilesystemRoots() {
	std::vector<std::string> roots;
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
	// From https://stackoverflow.com/a/57748722
	char* szDrives = new char[MAX_PATH]();
	if (GetLogicalDriveStringsA(MAX_PATH, szDrives));
	for (int i = 0; i < 100; i += 4)
		if (szDrives[i] != (char)0)
			roots.push_back(std::string{ szDrives[i],szDrives[i + 1],szDrives[i + 2] });
	delete[] szDrives;
#else
	roots.push_back("/"); // FIXME is this sensible?
#endif
	return roots;
}

std::string getFileLastWriteString(fs::path file) {
	// From https://stackoverflow.com/a/58237530
	try {
		using namespace std::chrono;
		auto ftime = fs::last_write_time(file);
		auto sctp = time_point_cast<system_clock::duration>(ftime - decltype(ftime)::clock::now() + system_clock::now());
		std::time_t tt = system_clock::to_time_t(sctp);
		std::tm* gmt = std::gmtime(&tt);
		std::stringstream buffer;
		buffer << std::put_time(gmt, "%a, %d.%m.%Y %H:%M");
		return buffer.str();
	}
	catch (...) {
		return "";
	}
}

static std::string bytesToString(size_t byteCount)
{
	// From https://stackoverflow.com/a/4975942
	std::string suf[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB" }; //Longs run out around EB
	if (byteCount == 0)
		return "0" + suf[0];
	size_t bytes = byteCount;
	int place = (int)std::floor(std::log(bytes) / std::log(1024));
	double num = bytes / std::pow(1024., place);
	std::stringstream ss;
	ss << std::fixed << std::setprecision(1) << num << " " << suf[place];
	return ss.str();
}

#pragma endregion

bool ImGui::DirectoryTree(ImGuiID id, std::string& selected, const char* root, ImFileSystemFlags flags, std::string ignoreIfStartsWith)
{
	ImGui::PushID(id);

	bool foldersSelectable = flags & ImFileSystemFlags_FolderSelect;
	bool filesSelectable = flags & ImFileSystemFlags_FileSelect;
	bool hideFiles = flags & ImFileSystemFlags_HideFiles;

	IM_ASSERT_USER_ERROR(!hideFiles || (hideFiles && !filesSelectable), "Invalid combination of ImDirectoryTreeFlags_FileSelect and ImDirectoryTreeFlags_HideFiles");

	std::vector<std::string> roots = root == NULL
		? getFilesystemRoots()
		: std::vector<std::string>{ std::string(root) };

	bool change = false;
	for (std::string croot : roots) {
		std::string stem = fs::path(croot).stem().string();
		if (stem.length() == 0) { stem = croot; }	// This is probably a real root folder
		if (ignoreIfStartsWith.find(stem[0]) != std::string::npos) { continue; }
		if (ImGui::TreeNode(stem.c_str())) {
			if (foldersSelectable && ImGui::IsItemClicked()) {	// FIXME better selection
				selected = croot;
				change = true;
			}

			std::vector<fs::path> folders;
			std::vector<fs::path> files;

			for (auto& child : fs::directory_iterator(croot, fs::directory_options::skip_permission_denied)) {
				if (child.is_directory()) {
					folders.push_back(child.path());
				}
				else if (child.is_regular_file()) {
					files.push_back(child.path());
				}
			}

			// Draw folders first
			for (const fs::path& folder : folders) {
				std::string path = folder.string();
				change = DirectoryTree(ImGui::GetID(path.c_str()), selected, path.c_str(), flags, ignoreIfStartsWith) || change;
			}
			if (!hideFiles) {
				// FIXME Indent files a little
				for (const fs::path& file : files) {
					if (filesSelectable) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
						std::string filename = file.filename().string();
						if (ImGui::Button(filename.c_str())) {
							change = true;
							selected = file.string();
						}
						ImGui::PopStyleVar(1);
						ImGui::PopStyleColor(1);
					}
					else {
						ImGui::Text(file.filename().string().c_str());
					}
				}
			}
			ImGui::TreePop();
		}
	}


	ImGui::PopID();
	return change;
}

bool ImGui::FilesystemDialogPopupModal(const char* str_id, std::string& selected, ImFileSystemFlags flags) {

	if (flags == 0) {
		IM_ASSERT_USER_ERROR(flags != 0, "You cannot select ImFileSystemFlags_None for popup!");
		return false;
	}

	const std::string hiddenFileInicators = ".$";

	const bool showHidden    = flags & ImFileSystemFlags_ShowHiddenEntries;
	const bool hideFiles     = flags & ImFileSystemFlags_HideFiles;
	const bool selectFolders = flags & ImFileSystemFlags_FolderSelect;
	const bool selectFiles   = flags & ImFileSystemFlags_FileSelect;
	const bool noDoubleClick = flags & ImFileSystemFlags_NoAcceptOnDoubleClick;
	const bool withPreview   = !(bool)(flags & ImFileSystemFlags_NoPreviews);

	ImGuiID id = ImGui::GetID(str_id);
	ImFilesystemDialoguePopup* popup = ImFilesystemDialoguePopup::FileDialogues.GetOrAddByKey(id);

	bool elementSelected = false;
	if (ImGui::BeginPopupModal(str_id)) {
		if (!popup->isOpen) {
			popup->isOpen = true;
			popup->currentSelected = selected;
		}
		if (popup->currentFolder == "" && selected == "") {
			popup->setCurrentFolder(ImFilesystemDialoguePopup::_Drives.front());
		}
		else if (popup->currentFolder == "" && fs::is_directory(selected)) {
			popup->setCurrentFolder(selected);
		}
		else if (popup->currentFolder == "" && fs::is_regular_file(selected)) {
			popup->setCurrentFolder(fs::path(selected).parent_path().string());
		}
		const fs::path currentFolder(popup->currentFolder);
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImVec2 okSize = CalcTextSize("OK", NULL) + style.FramePadding * 2.0f;
		const ImVec2 cancelSize = CalcTextSize("Cancel", NULL) + style.FramePadding * 2.0f;

		bool shouldSetNewFolderFocus = false;	// This indicates if new folder icon was pressed in this frame

		if (ImGui::BeginTable("##Layout", 2, ImGuiTableFlags_Resizable, ImVec2(0, 100))) {
			// -- Left Column
			ImGui::TableNextColumn();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
			// Drives
			ImGui::TextUnformatted("Drives");
			for (const std::string& drive : ImFilesystemDialoguePopup::_Drives) {
				bool selected = drive == currentFolder.root_name().string() + "\\";
				std::string label = std::string(ICON_FA_HDD) + " " + drive;
				if (selected) { ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)); }
				if (ImGui::Button(label.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
					popup->setCurrentFolder(drive); 
				}
				if (selected) { ImGui::PopStyleColor(); }
			}

			// Bookmarks
			ImGui::Dummy(ImVec2(0.f, 20.f));
			const ImVec2 buttonSize = ImGui::CalcTextSize("A", NULL) + style.FramePadding * 2.0f;
			ImGui::Text("%s Bookmarks", ICON_FA_BOOKMARK);
			ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - buttonSize.x);
			if (ImGui::Button(ICON_FA_PLUS)) {
				ImFilesystemDialoguePopup::AddBookmark(currentFolder.string());
			}
			std::string bmToRemove = "";
			for (const std::string& bm : ImFilesystemDialoguePopup::Bookmarks) {
				std::string name = fs::path(bm).filename().string();
				if (name == "") { name = bm; }	// We need this because Root directories do not have a filename
				if (ImGui::Button(name.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth() - buttonSize.x - style.ItemSpacing.x, 0))) {
					popup->setCurrentFolder(bm);
				}
				if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.65f) { ImGui::SetTooltip(bm.c_str()); }	// Delayed full path tooltip
				ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - buttonSize.x);
				if(ImGui::Button(ICON_FA_MINUS)) {
					bmToRemove = bm;
				}
			}
			if (bmToRemove != "") { ImFilesystemDialoguePopup::RemoveBookmark(bmToRemove); }
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor();

			// -- End Left Column

			// --- Right Column Column (Filesystem viewer)
			ImGui::TableNextColumn();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
			if (ImGui::Button(ICON_FA_ARROW_LEFT)) {
				popup->historyBack();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_ARROW_RIGHT)) {
				popup->historyForward();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_ARROW_UP) && currentFolder.has_parent_path()) {
				popup->setCurrentFolder(currentFolder.parent_path().string());
			}
			ImGui::SameLine();
			ImGui::Button(ICON_FA_SYNC);	// Currently useless (Maybe needed if I decide to cache folder data)
			ImGui::PopStyleVar();
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_FOLDER_PLUS)) {
				popup->_creatingNewFolder = true;
				popup->_newFolder = "New folder"; // FIXME: check if this name is unique
				shouldSetNewFolderFocus = true;
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputText("##CurrentFolder", popup->_currentFolderUserInput, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
				popup->setCurrentFolder(popup->_currentFolderUserInput);
			}

			if (fs::exists(currentFolder)) {
				if (withPreview) {
					ImGuiTabBarFlags previewFlags = ImGuiTableFlags_Resizable;
					ImGui::BeginTable("##SelectorLayout", 2, previewFlags);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
				}
				ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
				ImVec2 tableSize(0, ImGui::GetContentRegionAvail().y - okSize.y - 2.0f * style.ItemSpacing.y);
				if (ImGui::BeginTable("##Selector", 3, flags, tableSize)) {
					ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
					ImGui::TableSetupColumn("Date Modified", ImGuiTableColumnFlags_NoSort);         // FIXME: Enable sorting for these two columns
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_NoSort);         // FIXME: Enable sorting for these two columns
					ImGui::TableHeadersRow();

					// Display contents of current folder
					std::vector<fs::path> folders;
					std::vector<fs::path> files;


					for (auto& child : fs::directory_iterator(currentFolder, fs::directory_options::skip_permission_denied)) {
						if (child.is_directory()) {
							folders.push_back(child.path());
						}
						else if (child.is_regular_file()) {
							files.push_back(child.path());
						}
					}

					// FIXME implement sorting for all columns
					ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
					if (sortSpecs && sortSpecs->SpecsDirty) {
						if (sortSpecs->Specs->SortDirection == ImGuiSortDirection_Ascending) {
							std::sort(folders.begin(), folders.end(), std::less());
						}
						else {
							std::sort(folders.begin(), folders.end(), std::greater());
						}
					}

					// Draw folders first
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.f, 0.5f));

					// Show link to parent folder
					if (currentFolder.parent_path() != currentFolder) {
						ImGui::TableNextColumn();									// Name
						const std::string label = std::string(ICON_FA_FOLDER_OPEN) + " ..";
						if (ImGui::Button(label.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
							if (selectFolders) { popup->currentSelected = currentFolder.string(); }
						}
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
							popup->setCurrentFolder(currentFolder.parent_path().string());
						}
						ImGui::TableNextColumn();									// Date Modified
						ImGui::TableNextColumn();									// Size
					}

					// Show new folder
					if (popup->_creatingNewFolder) {
						ImGui::TableNextColumn();
						if (shouldSetNewFolderFocus) { SetKeyboardFocusHere(); }
						InputText("##Foldername", popup->_newFolder, 256);			// Name
						if (IsKeyPressed(GLFW_KEY_ESCAPE)) {
							popup->_creatingNewFolder = false;
						}
						if (IsItemDeactivatedAfterEdit()) {
							try {
								fs::create_directory(currentFolder / popup->_newFolder);
							}
							catch (...) {
								std::cerr << "Could not create folder \"" + (fs::path(currentFolder) / popup->_newFolder).string() + "\"\n";
							}
							popup->_creatingNewFolder = false;
						}
						ImGui::TableNextColumn();									// Date Modified
						ImGui::TableNextColumn();									// Size
					}

					// Show all folders
					for (const fs::path& folder : folders) {
						const std::string path = folder.string();
						const std::string name = folder.filename().string();
						const std::string label = std::string(ICON_FA_FOLDER_OPEN) + " " + name;
						if (!showHidden && hiddenFileInicators.find(name[0]) != std::string::npos) { continue; }	// Skip hidden files/folders
						ImGui::TableNextColumn();									// Name
						if (ImGui::Button(label.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)) && selectFolders) {
							if (selectFolders) { popup->currentSelected = currentFolder.string(); }
						}
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
							popup->setCurrentFolder(path);
						}
						ImGui::TableNextColumn();									// Date Modified
						ImGui::TableNextColumn();									// Size
					}

					// Show all files
					if (!hideFiles) {
						for (const fs::path& file : files) {
							ImGui::TableNextColumn();								// Name
							std::string path = file.string();
							std::string name = file.filename().string();
							std::string icon = std::string(ICON_FA_FILE);
							std::string extension = file.extension().string();
							std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
							if (popup->FileIcons.find(extension) != popup->FileIcons.end()) {
								icon = popup->FileIcons[extension];
							}
							const std::string label = icon + " " + name;
							if (ImGui::Button(label.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
								if (selectFiles) { popup->currentSelected = file.string(); }
								// Check if a preview is available
								popup->setPreviewFunction(file.string());
							}
							if (!noDoubleClick && selectFiles && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
								ImGui::CloseCurrentPopup();
								popup->isOpen = false;
								popup->_history.clear();
								selected = file.string();
								elementSelected = true;
							}
							ImGui::TableNextColumn();								// Date Modified
							ImGui::Text(getFileLastWriteString(file).c_str());
							ImGui::TableNextColumn();								// Size
							try {
								const std::string size = bytesToString(fs::file_size(file));
								ImGui::Text("%s", size.c_str());
							}
							catch (...) {}
						}
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleColor();

					ImGui::EndTable();
				}
				if (withPreview) {
					ImGui:TableNextColumn();
					if (popup->currentPreview) {
						popup->currentPreview->draw();
					}
					ImGui::EndTable();
				}
			}
			else if (!fs::exists(currentFolder)) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
				Text("%s ", ICON_FA_EXCLAMATION_TRIANGLE);
				ImGui::PopStyleColor();
				ImGui::SameLine();
				Text("Folder \"%s\" does not exist", currentFolder.string().c_str());
			}
			// Draw current result
			ImGui::TextUnformatted(popup->currentSelected.c_str());
			ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - okSize.x - cancelSize.x - 2.0f * style.ItemInnerSpacing.x);
			if (popup->currentSelected == "") {	// Disable OK button if nothing is selected
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button("OK")) {
				ImGui::CloseCurrentPopup();
				popup->isOpen = false;
				popup->_history.clear();
				selected = popup->currentSelected;
				elementSelected = true;
			}
			if (popup->currentSelected == "") {
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
				popup->isOpen = false;
				popup->_history.clear();
				elementSelected = false;
			}
			// --- End Right/Center Column

			ImGui::EndTable();
		}
		ImGui::EndPopup();
	}

	return elementSelected;
}

bool ImGui::FilePathInput(const char* label, std::string& file, size_t maxSize, const char* msg)
{

	return FileSystemPathInputEx(label, file, maxSize, msg, ImFileSystemFlags_FileSelect);
}

bool ImGui::FolderPathInput(const char* label, std::string& file, size_t maxSize, const char* msg)
{
	return FileSystemPathInputEx(label, file, maxSize, msg, ImFileSystemFlags_HideFiles | ImFileSystemFlags_FolderSelect);
}

bool ImGui::FileSystemPathInputEx(const char* label, std::string& path, size_t maxSize, const char* msg, ImFileSystemFlags flags)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	if (window->SkipItems)
		return false;

	ImGui::PushID(label);
	const ImVec2 buttonSize = ImGui::CalcTextSize("Open", NULL) + g.Style.FramePadding * 2.0f;
	const float width = window->DC.ItemWidth - buttonSize.x - g.Style.ItemInnerSpacing.x;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, g.Style.ItemInnerSpacing);
	ImGui::PushItemWidth(width);
	bool change = ImGui::InputText("##path", (char*)path.c_str(), maxSize, ImGuiInputTextFlags_ReadOnly);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Open")) {
		ImGui::OpenPopup(msg);
	}
	ImGui::PopStyleVar();
	ImGui::SameLine();
	ImGui::TextUnformatted(label, FindRenderedTextEnd(label));

	if (FilesystemDialogPopupModal(msg, path, flags)) {
		change = true;
	}

	ImGui::PopID();

	return change;
}

ImGui::ImFilesystemDialoguePopup::ImFilesystemDialoguePopup() :
	isOpen(false),
	currentFolder(""),
	currentSelected(""),
	currentPreview(nullptr),
	_creatingNewFolder(false),
	_newFolder(""),
	_historyPos(0)
{
	// Initialize global stuff
	if (_FirstCall) { 
		_LoadBookmarks();
		_Drives = getFilesystemRoots();
		std::shared_ptr<ImagePreview> imagePreview = std::make_shared<ImagePreview>();
		PreviewFunctions.insert({ ".png", imagePreview });
		PreviewFunctions.insert({ ".jpg", imagePreview });
	}
}

void ImGui::ImFilesystemDialoguePopup::setCurrentFolder(std::string folder)
{
	if (fs::exists(currentFolder) && fs::is_directory(currentFolder)) {
		_history.resize(_historyPos + 1);
		_history.push_back(folder);
		_historyPos = _history.size() - 1;
	}
	currentFolder = folder;
	_currentFolderUserInput = folder;
}

void ImGui::ImFilesystemDialoguePopup::setPreviewFunction(std::string file)
{
	std::string ext = fs::path(file).extension().string();
	if (PreviewFunctions.find(ext) != PreviewFunctions.end()) {
		currentPreview = PreviewFunctions[ext];
		currentPreview->selected(file);
	}
	else {
		currentPreview = nullptr;
	}
}

void ImGui::ImFilesystemDialoguePopup::historyBack()
{
	if (_historyPos != 0) {
		currentFolder = _history[_historyPos - 1];
		_currentFolderUserInput = currentFolder;
		_historyPos -= 1;
	}
}

void ImGui::ImFilesystemDialoguePopup::historyForward()
{
	if (_historyPos + 1 < _history.size()) {
		currentFolder = _history[_historyPos + 1];
		_currentFolderUserInput = currentFolder;
		_historyPos += 1;
	}
}

void ImGui::ImFilesystemDialoguePopup::_LoadBookmarks()
{
	if (fs::exists(IMGUI_BOOKMARKS_FILE)) {
		std::ifstream f(IMGUI_BOOKMARKS_FILE);
		IM_ASSERT(f.is_open());
		std::string line;
		while (std::getline(f, line)) {
			Bookmarks.insert(line);
		}
	}
}

void ImGui::ImFilesystemDialoguePopup::AddBookmark(std::string path)
{
	if (Bookmarks.find(path) == Bookmarks.end()) {
		Bookmarks.insert(path);
		std::ofstream f(IMGUI_BOOKMARKS_FILE);
		IM_ASSERT(f.is_open());
		for (std::string bm : Bookmarks) {
			f << bm << std::endl;
		}
	}
}

void ImGui::ImFilesystemDialoguePopup::RemoveBookmark(std::string path)
{
	IM_ASSERT(Bookmarks.find(path) != Bookmarks.end());
	Bookmarks.erase(Bookmarks.find(path));
	std::ofstream f(IMGUI_BOOKMARKS_FILE);
	IM_ASSERT(f.is_open());
	for (std::string bm : Bookmarks) {
		f << bm << std::endl;
	}
}

void ImGui::ImagePreview::selected(std::string path)
{
	try {
		mPreviewTexture = std::make_unique<gl::LargeTexture>(path, gl::TextureFlags_No_Mipmap);
	} catch(std::exception& ex) {
		mPreviewTexture = nullptr;
	}
	mPath = path;
}

void ImGui::ImagePreview::draw()
{
	ImGui::TextUnformatted(mPath.c_str());
	if (mPreviewTexture != nullptr) {
		ImVec2 space = ImGui::GetContentRegionAvail();
		float s = std::min(1.0f, space.x / mPreviewTexture->cols);
		ImGui::Image(mPreviewTexture.get(), ImVec2(mPreviewTexture->cols, mPreviewTexture->rows) * s);
	}
	else {
		ImGui::TextUnformatted("Could not load image");
	}
}
