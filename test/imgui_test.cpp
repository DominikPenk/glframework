#include <glpp/renderer.hpp>
#include <glpp/imgui.hpp>
#include <glpp/texture.hpp>

int main() {
	auto cam = std::make_shared<gl::Camera>(
		glm::vec3(0.0f, 4.0f, 10.0f),
		glm::vec3(0, 0, 0));
	cam->Far = -1.f;
	cam->Near = 0.01f;
	cam->ScreenHeight = 720;
	cam->ScreenWidth = 1024;

	gl::Renderer renderer(1024, 720, cam, "Test Window", false);
	renderer.showOutliner = false;

	auto lena = std::make_shared<gl::Texture>(std::string(TEST_DIR) + "lena.jpg");
	glm::vec2 center(100, 100);
	glm::vec2 size(250, 100);

	while (!renderer.shouldClose()) {
		renderer.startFrame();

		ImGui::BeginCanvasFullscreen("Canvas");

		auto dl = ImGui::GetCurrentWindow()->DrawList;
		dl->AddImage(*lena, ImVec2(0, 0), ImVec2(cam->ScreenWidth, cam->ScreenHeight), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::AxisAlignedBoundingBox("BB", &center.x, &size.x, IM_COL32(255, 0, 0, 255));

		ImGui::EndCanvas();

		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}