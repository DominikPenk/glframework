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
	float angle = 0.f;

	while (!renderer.shouldClose()) {
		renderer.startFrame();

		ImGui::BeginCanvasFullscreen("Canvas");

		auto dl = ImGui::GetCurrentWindow()->DrawList;
		
		// Add a rotated version of lena
		glm::mat2 R(1);
		R[0][0] = R[1][1] = std::cos(glm::radians(angle));
		R[0][1] = std::sin(glm::radians(angle));
		R[1][0] = -R[0][1];
		
		dl->AddImageQuad(*lena,
			glm2ImGui(R * glm::vec2(-lena->cols, -lena->rows) * 0.5f + center),
			glm2ImGui(R * glm::vec2( lena->cols, -lena->rows) * 0.5f + center),
			glm2ImGui(R * glm::vec2( lena->cols,  lena->rows) * 0.5f + center),
			glm2ImGui(R * glm::vec2(-lena->cols,  lena->rows) * 0.5f + center),
			ImVec2(0, 1),
			ImVec2(1, 1),
			ImVec2(1, 0),
			ImVec2(0, 0));


		ImGui::AxisAlignedBox("BB", &center.x, &size.x, IM_COL32(255, 0, 0, 255));

		ImGui::Rotation("BBoxrot", &angle, &center.x, 100.f, IM_COL32(0, 255, 0, 255));

		ImGui::EndCanvas();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("Bounding Box", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
		ImGui::Text("Position: (%d, %d)", (int)center.x, (int)center.y);
		ImGui::Text("Size:     (%d, %d)", (int)size.x, (int)size.y);
		ImGui::Text("Angle:    %.3f", angle);
		ImGui::End();

		renderer.endFrame();

	}

	return EXIT_SUCCESS;
}