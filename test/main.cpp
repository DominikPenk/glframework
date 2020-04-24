#include <glm/gtx/string_cast.hpp>

#include "renderer.hpp"
#include "camera.hpp"
#include "controls.hpp"
#include "splinecurves.hpp"
#include "openmesh_mesh.h"


#include "imgui.h"

#include "imgui3d/imgui_3d.h"
#include "imgui3d/imgui_3d_editor_widgets.h"
#include "imgui3d/imgui_3d_surfaces.h"

#ifdef WITH_OPENMESH
#include "openmesh_ext.h"
#include "OpenMeshExtension/MeshDebugWindow.h"
#endif

#include "texture.hpp"

int main(int argc, const char* argv[]) {
	auto cam = std::make_shared<gl::Camera>(
		glm::vec3(0.0f, -30.0f, 2.0f),
		glm::vec3(0, 0, 0));
	cam->Far = 2600;

	gl::OrbitControl control(cam);

	gl::Renderer renderer(800, 600, cam, "Test Window", true);

	//auto spline = renderer.addMesh<gl::BSplineSurfaceMesh>("Lens", approximateSphere(16, 32.5, 3.25));
	auto coo = renderer.addMesh<gl::CoordinateFrame>("Coordinate Frame");
	
	renderer.showDebug = true;
	renderer.showMeshWatch = true;

	glm::vec3 p0(0);
	glm::vec4 p1(10, 4, 0, 1);
	glm::vec4 d(0, 0, -1, 0);
	glm::vec4 angles(glm::radians(45.0f), 0, 0, 1);
	glm::mat4 T(1);

	int subdivs = 3;
	int gridSize = 4;
	glm::vec4 params(.1f, -.23f, -.3f, 0.f);

	auto window = renderer.addUIWindow("Surface Settings", [&](gl::Renderer* env) {
		ImGui::InputFloat4("Parameters", &params[0]);
		ImGui::DragInt("GridSize", &gridSize, 1.f, 1, 10);
		ImGui::DragInt("Subdivisions", &subdivs, 1.f, 1, 10);
	});

	auto omesh = renderer.addMesh<gl::OpenMeshMesh>("OpenMesh Mesh");
	OpenMesh::TriangleMesh3f::VertexHandle vhandle[8];
	vhandle[0] = omesh->data().add_vertex(OpenMesh::TriangleMesh3f::Point(-1, -1, 1));
	vhandle[1] = omesh->data().add_vertex(OpenMesh::TriangleMesh3f::Point(1, -1, 1));
	vhandle[2] = omesh->data().add_vertex(OpenMesh::TriangleMesh3f::Point(1, 1, 1));
	vhandle[3] = omesh->data().add_vertex(OpenMesh::TriangleMesh3f::Point(-1, 1, 1));
	vhandle[4] = omesh->data().add_vertex(OpenMesh::TriangleMesh3f::Point(-1, -1, -1));
	vhandle[5] = omesh->data().add_vertex(OpenMesh::TriangleMesh3f::Point(1, -1, -1));
	vhandle[6] = omesh->data().add_vertex(OpenMesh::TriangleMesh3f::Point(1, 1, -1));
	vhandle[7] = omesh->data().add_vertex(OpenMesh::TriangleMesh3f::Point(-1, 1, -1));
	

	// generate (quadrilateral) faces
	std::vector<OpenMesh::TriangleMesh3f::VertexHandle>  face_vhandles;
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	omesh->data().add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[4]);
	omesh->data().add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[5]);
	omesh->data().add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[6]);
	omesh->data().add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[7]);
	omesh->data().add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[4]);
	omesh->data().add_face(face_vhandles);

#ifdef _DEBUG
	for (auto vh : vhandle) {
		omesh->data().watch(vh);
	}
#endif
	
	//std::dynamic_pointer_cast<gl::MeshDebugWindow>(renderer.mMeshWatch)->addBreakPoint(omesh, vhandle[1]);

	std::mutex m;
	std::condition_variable cond_var;
	omesh->addBreakpoint(vhandle[1], [&]() {
		std::cout << "Triggered Breakpoint ... ";
		std::unique_lock<std::mutex> lock(m);
		cond_var.wait(lock);
		std::cout << "Resume!" << std::endl;
	});

	std::thread backgroundWorker([&]() {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		std::cout << "Trigger Breakpoint!" << std::endl;
		omesh->data().point(vhandle[1]);
	});

	while (!renderer.shouldClose()) {
		renderer.startFrame();
		control.update(cam);
		//ImGui3D::DirectionalLight(&d[0], &p1[0]);
		ImGui3D::Spotlight(d, p1, angles[0]);
		ImGui3D::CubeMap(p0);
		ImGui3D::CamerViewDirection(renderer.camera()->viewMatrix);

		if (ImGui::IsKeyReleased(GLFW_KEY_N)) {
			std::unique_lock<std::mutex> lock(m);
			cond_var.notify_all();
		}

		ImGui3D::ParametricSurface([&](float u, float v) {
			return glm::vec3(u, v, params.x * u * u + params.y * u * v + params.z * v * v + params.w);
			}, glm::vec4(.9, .1, .1, .5), ImVec4(-2, -2, 2, 2), ImVec2(gridSize, gridSize), subdivs);
		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}