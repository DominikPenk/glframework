#include <glpp/renderers/editor.hpp>

#include <glpp/logging.hpp>
#include <glpp/meshes.hpp>

int main(int argc, const char* argv[]) {
	
	gl::Editor editor("Editor Sample");
	editor.viewportCamera->lookAt(glm::vec3(0), glm::vec3(0, 5, 15));

	editor.resetUILayout();

	auto teapot = editor.addObject<gl::TriangleMesh>("Teapot", std::string(SAMPLE_DIR) + "teapot.obj");
	LOG_SUCCESS_OR_ERROR(teapot != nullptr, "Loaded teapot mesh", "Could not load teapot mesh");
	LOG_WARNING("Just a sample warning, nothing to fear!");
	for (int i = 0; i < 4; ++i) {
		LOG_ONCE("This is only logged %s!", "once");
		LOG_EVERY_N(2, "This is logged only if #calls.mod(%d) == 0.", 2);
	}
	editor.run();

	return EXIT_SUCCESS;
}