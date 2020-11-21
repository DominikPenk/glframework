#include <glm/gtx/string_cast.hpp>

#include <glpp/renderer.hpp>
#include <glpp/camera.hpp>
#include <glpp/controls.hpp>
#include <glpp/meshes.hpp>
#include <glpp/imgui.hpp>

#include <glpp/imgui3d/imgui_3d.h>
#include <glpp/imgui3d/imgui_3d_editor_widgets.h>
#include <glpp/imgui3d/imgui_3d_surfaces.h>
#include <glpp/texture.hpp>


int main(int argc, const char* argv[]) {
	auto cam = std::make_shared<gl::Camera>(
		glm::vec3(0.0f, 5.0f, 6.0f),
		glm::vec3(0, 0, 0));
	cam->Far = -1.f;
	cam->Near = 0.01f;


	gl::FlyingControl control(cam);

	gl::Renderer renderer(800, 600, cam, "Test Window", true);

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
	std::vector<glm::vec3> vertices = {
		{ -1, -1,  1 },
		{  1, -1,  1 },
		{  1,  1,  1 },
		{ -1,  1,  1 },
		{ -1, -1, -1 },
		{  1, -1, -1 },
		{  1,  1, -1 },
		{ -1,  1, -1 }
	};
	std::vector<glm::ivec3> faces = {
		{ 0, 1, 2 }, { 0, 2, 3 },
		{ 7, 6, 5 }, { 7, 5, 4 },
		{ 1, 0, 4 }, { 1, 4, 5 },
		{ 2, 1, 5 }, { 2, 5, 6 },
		{ 3, 2, 6 }, { 3, 6, 7 },
		{ 0, 3, 7 }, { 0, 7, 4 }
	};
	auto mesh = renderer.addMesh<gl::TriangleMesh>("OpenMesh Mesh", vertices, faces);
	
	while (!renderer.shouldClose()) {
		renderer.startFrame();
		control.update(cam);
		//ImGui3D::DirectionalLight(&d[0], &p1[0]);
		ImGui3D::Spotlight(d, p1, angles[0]);
		ImGui3D::CubeMap(p0);
		ImGui3D::CamerViewDirection(renderer.camera()->viewMatrix);

		ImGui3D::ParametricSurface([&](float u, float v) {
			return glm::vec3(u, v, params.x * u * u + params.y * u * v + params.z * v * v + params.w);
			}, glm::vec4(.9, .1, .1, .5), ImVec4(-2, -2, 2, 2), ImVec2(gridSize, gridSize), subdivs);
		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}