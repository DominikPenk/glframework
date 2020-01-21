#include <glm/gtx/string_cast.hpp>

#include "renderer.hpp"
#include "camera.hpp"
#include "controls.hpp"
#include "splinecurves.hpp"


#include "imgui.h"

#include "imgui3d/imgui_3d.h"
#include "imgui3d/imgui_3d_editor_widgets.h"

#include "texture.hpp"

int main(int argc, const char* argv[]) {
	auto cam = std::make_shared<gl::Camera>(
		glm::vec3(0.0f, -30.0f, 2.0f),
		glm::vec3(0, 0, 0));
	cam->Far = 2600;

	gl::OrbitControl control(cam);

	gl::Renderer renderer(800, 600, cam);

	//auto spline = renderer.addMesh<gl::BSplineSurfaceMesh>("Lens", approximateSphere(16, 32.5, 3.25));
	auto coo = renderer.addMesh<gl::CoordinateFrame>("Coordinate Frame");
	
	renderer.showDebug = true;

	glm::vec3 p0(0);
	glm::vec4 p1(10, 4, 0, 1);
	glm::vec4 d(0, 0, -1, 0);
	glm::vec4 angles(glm::radians(45.0f), 0, 0, 1);
	glm::mat4 T(1);

	while (!renderer.shouldClose()) {
		renderer.startFrame();
		control.update(cam);

		//ImGui3D::GImGui3D->currentDrawList()->AddScreenAlignedCircle(glm::vec4(0, 0, 0, 1), 25, 4, glm::vec4(1, 0, 0, 1), 24);
		//ImGui3D::GImGui3D->currentDrawList()->AddScreenAlignedQuad(glm::vec4(5, 5, 0, 1), ImVec2(25, 50), 4, glm::vec4(1, 0, 0, 1));
		//ImGui3D::GImGui3D->currentDrawList()->AddLine(glm::vec4(10, 5, 0, 1), glm::vec4(25, 5, 10, 1), 4, glm::vec4(1, 0, 0, 1));

		//ImGui3D::DirectionalLight(&d[0], &p1[0]);
		ImGui3D::Spotlight(d, p1, angles[0]);
		ImGui3D::CubeMap(p0);
		ImGui3D::CamerViewDirection(renderer.camera()->viewMatrix);

		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}