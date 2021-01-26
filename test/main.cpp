#include <glpp/renderers/editor.hpp>

#include <glpp/meshes.hpp>

int main(int argc, const char* argv[]) {
	
	gl::Editor editor("Test Window");
	editor.viewportCamera->lookAt(glm::vec3(0), glm::vec3(0, 1, 5));

	auto teapot = editor.addObject<gl::TriangleMesh>("Teapot", std::string(TEST_DIR) + "teapot.obj");

	//editor.resetUILayout();
	editor.showDebug = false;

	editor.run();
	

	return EXIT_SUCCESS;
}