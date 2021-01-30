#include <glpp/renderers/immediate_renderer.hpp>

#include <glpp/meshes.hpp>
#include <glpp/imgui.hpp>
#include <glpp/imgui3d/imgui_3d.h>

int main(int argc, const char* argv[]) {

	gl::ImmediateRenderer renderer("Intermediate Renderer Sample");
	renderer.viewportCamera->lookAt(glm::vec3(0), glm::vec3(0, 5, 15));

	auto teapot = renderer.addObject<gl::TriangleMesh>(std::string(SAMPLE_DIR) + "teapot.obj");

	while (!renderer.shouldClose()) {
		renderer.startFrame();

		ImGui::ShowDemoWindow();


		// You can render all objects stored by the renderer like this:
		// renderer.renderObjects(renderer.viewportCamera);
		teapot->render(renderer.viewportCamera);
		ImGui3D::TransformGizmo(teapot->ModelMatrix);
		
		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}