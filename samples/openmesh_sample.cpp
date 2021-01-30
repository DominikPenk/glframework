#include <glpp/camera.hpp>
#include <glpp/controls.hpp>
#include <glpp/meshes.hpp>
#include <glpp/texture.hpp>
#include <glpp/framebuffer.hpp>
#include <glpp/intermediate.h>
#include <glpp/renderers/editor.hpp>

#include <iostream>


int main(int argc, const char* argv[]) {
	gl::Editor editor(1024, 720, "Test Window");
	editor.getViewportCamera()->lookAt(glm::vec3(0), glm::vec3(0, 5, 15));

	auto mesh = editor.addObject<gl::OpenMeshMesh>("Teapot", std::string(SAMPLE_DIR) + "teapot.obj");

	editor.run();

	return EXIT_SUCCESS;
}