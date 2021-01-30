#include <glpp/renderers/editor.hpp>

#include <glpp/meshes.hpp>

int main(int argc, const char* argv[]) {
	
	gl::Editor editor("Editor Sample");
	editor.viewportCamera->lookAt(glm::vec3(0), glm::vec3(0, 5, 15));

	auto teapot = editor.addObject<gl::TriangleMesh>("Teapot", std::string(SAMPLE_DIR) + "teapot.obj");

	editor.run();

	return EXIT_SUCCESS;
}