#include <glpp/renderers/editor.hpp>

#include <glpp/logging.hpp>
#include <glpp/meshes.hpp>

int main(int argc, const char* argv[]) {
	
	gl::Editor editor("Editor Sample");
	editor.viewportCamera->lookAt(glm::vec3(0), glm::vec3(0, 5, 15));

	auto teapot = editor.addObject<gl::TriangleMesh>("Teapot", std::string(SAMPLE_DIR) + "teapot.obj");
	LOG_SUCCESS_OR_ERROR(teapot != nullptr, "Loaded teapot mesh", "Could not load teapot mesh");
	editor.run();

	return EXIT_SUCCESS;
}