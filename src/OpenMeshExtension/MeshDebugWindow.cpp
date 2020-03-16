#include "MeshDebugWindow.h"

#include "../renderer.hpp"
#include "../openmesh_mesh.h"
#include "IconsFontAwesome5.h"

#include <imgui.h>

static inline void BreakPoint() {

}

gl::MeshDebugWindow::MeshDebugWindow() :
	UIWindow("Mesh Watch"),
	breakPointTriggered(false)
{
	mBreakPointCallback = [this]() {
		std::cout << "Breakpoint triggered ... ";
		breakPointTriggered = true;
		std::unique_lock<std::mutex> lock(mutex);
		breakpoint.wait(lock);
		std::cout << "Resume" << std::endl;
	};
}

void gl::MeshDebugWindow::onDraw(gl::Renderer* env)
{
#ifdef _DEBUG
	std::vector<std::shared_ptr<OpenMeshMesh>> watchableMeshes = env->getMeshes<gl::OpenMeshMesh>();


	// Get all OpenMesh meshes the renderer has to over
	for (auto mesh : watchableMeshes) {
		ImGui::Text(mesh->name.c_str());
		mesh->setBreakPointCheck(false);
	}

	int tbl_flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollFreezeTopRow | ImGuiTableFlags_SizingPolicyMaskX_;
	
	// Check if anyone is waiting
	if (breakPointTriggered && ImGui::Button("Resume")) {
		breakPointTriggered = false;
		std::lock_guard<std::mutex> guard(mutex);
		breakpoint.notify_all();
	}


	if (ImGui::TreeNode("Watched Vertex Handles")) {

		if (ImGui::BeginTable("##VertexHandles", 6, tbl_flags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TableHeader("Mesh");
			ImGui::TableSetColumnIndex(1);
			ImGui::TableHeader("   Index");
			ImGui::TableSetColumnIndex(2);
			ImGui::TableHeader("is_valid");
			ImGui::TableSetColumnIndex(3);
			ImGui::TableHeader("is_bounary");
			ImGui::TableSetColumnIndex(4);
			ImGui::TableHeader(" valence");

			int row = 0; 
			for (auto mesh : watchableMeshes) {
				const OpenMesh::TriangleMesh3f& obj = mesh->data();
				for (auto vh : obj.watchedVertexHandles) {
					ImGui::PushID(row++);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(mesh->name.c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%*d", 8, vh.idx());
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("   %s", (vh.is_valid() ? ICON_FA_CHECK : ICON_FA_TIMES));
					ImGui::TableSetColumnIndex(3);
					ImGui::Text("    %s", (obj.is_boundary(vh) ? ICON_FA_CHECK : ICON_FA_TIMES));
					ImGui::TableSetColumnIndex(4);
					ImGui::Text("%*d", 8, obj.valence(vh));
					ImGui::TableSetColumnIndex(5);
					if (ImGui::Button("Remove")) {
						mesh->stopWatch(vh);
					}
					ImGui::PopID();

				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Watched Edge Handles")) {

		if (ImGui::BeginTable("##EdgeHandles", 4, tbl_flags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TableHeader("Mesh");
			ImGui::TableSetColumnIndex(1);
			ImGui::TableHeader("Index");
			ImGui::TableSetColumnIndex(2);
			ImGui::TableHeader("is_valid");

			int row = 0;
			for (auto mesh : watchableMeshes) {
				const OpenMesh::TriangleMesh3f& obj = mesh->data();
				for (auto eh : obj.watchedEdgeHandles) {
					ImGui::PushID(row++);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(mesh->name.c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%d", eh.idx());
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%s", (eh.is_valid() ? ICON_FA_CHECK : ICON_FA_TIMES));
					ImGui::TableSetColumnIndex(3);
					if (ImGui::Button("Remove")) {
						mesh->stopWatch(eh);
					}
					ImGui::PopID();

				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Watched Halfedge Handles")) {

		if (ImGui::BeginTable("##HalfedgeHandles", 5, tbl_flags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TableHeader("Mesh");
			ImGui::TableSetColumnIndex(1);
			ImGui::TableHeader("Index");
			ImGui::TableSetColumnIndex(2);
			ImGui::TableHeader("is_valid");
			ImGui::TableSetColumnIndex(3);
			ImGui::TableHeader("is_boundary");

			int row = 0;
			for (auto mesh : watchableMeshes) {
				const OpenMesh::TriangleMesh3f& obj = mesh->data();
				for (auto heh : obj.watchedHalfedgeHandles) {
					ImGui::PushID(row++);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(mesh->name.c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%d", heh.idx());
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%s", (heh.is_valid() ? ICON_FA_CHECK : ICON_FA_TIMES));
					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%s", (obj.is_boundary(heh) ? ICON_FA_CHECK : ICON_FA_TIMES));
					ImGui::TableSetColumnIndex(4);
					if (ImGui::Button("Remove")) {
						mesh->stopWatch(heh);
					}
					ImGui::PopID();

				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Watched Face Handles")) {

		if (ImGui::BeginTable("##EdgeHandles", 4, tbl_flags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TableHeader("Mesh");
			ImGui::TableSetColumnIndex(1);
			ImGui::TableHeader("Index");
			ImGui::TableSetColumnIndex(2);
			ImGui::TableHeader("is_valid");

			int row = 0;
			for (auto mesh : watchableMeshes) {
				const OpenMesh::TriangleMesh3f& obj = mesh->data();
				for (auto fh : obj.watchedFaceHandles) {
					ImGui::PushID(row++);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(mesh->name.c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%d", fh.idx());
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%s", (fh.is_valid() ? ICON_FA_CHECK : ICON_FA_TIMES));
					ImGui::TableSetColumnIndex(3);
					if (ImGui::Button("Remove")) {
						mesh->stopWatch(fh);
					}
					ImGui::PopID();

				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}

	for (auto mesh : watchableMeshes) {
		mesh->setBreakPointCheck(true);
	}
#endif
}
