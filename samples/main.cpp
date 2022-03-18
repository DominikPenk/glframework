#include <glpp/renderers/editor.hpp>

#include <glpp/logging.hpp>
#include <glpp/meshes.hpp>

int main(int argc, const char* argv[]) {
	
	gl::Editor editor("Editor Sample");
	editor.getViewportCamera()->lookAt(glm::vec3(0), glm::vec3(0, 5, 15));

	editor.resetUILayout();

	editor.getViewport()->registerRenderHook(gl::RenderHook::PostMeshDrawing, [&](const gl::ViewportEditorWindow*) {
		LOG_EVERY_N(100, "This is logged every 100 frames after objects are drawn");
	});

	auto teapot = editor.addObject<gl::TriangleMesh>("Teapot", std::string(SAMPLE_DIR) + "teapot.obj");
	LOG_SUCCESS_OR_ERROR(teapot != nullptr, "Loaded teapot mesh", "Could not load teapot mesh");
	LOG_WARNING("Just a sample warning, nothing to fear!");
	for (int i = 0; i < 4; ++i) {
		LOG_ONCE("This is only logged once!");
		LOG_ERROR_EVERY_N(2, "This is logged only if #calls.mod(%d) == 0.", 2);
	}

	editor.addEditorWindow("Lambda Window", [](gl::Editor*) {
		ImGui::TextUnformatted("This is a lambda window");
	});

	editor.run();

	return EXIT_SUCCESS;
}