#include <glpp/2d/canvas.hpp>
#include <glpp/2d/sprite.hpp>
#include <glpp/2d/selector.hpp>

#include <glpp/renderer.hpp>
#include <glpp/imgui.hpp>

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

	gl::Canvas canvas(0, 0, cam->ScreenWidth, cam->ScreenHeight);

	auto sprite1 = canvas.addElement<gl::Sprite>("Block");
	sprite1->color = glm::vec3(1, 0, 0);
	sprite1->size = glm::vec2(250, 125);
	sprite1->position = glm::vec2(100, 500);
	sprite1->layer = 100;

	auto sprite2 = canvas.addElement<gl::Sprite>("Image", std::string(TEST_DIR) + "lena.jpg");
	sprite2->position = glm::vec2(100, 0);
	sprite2->layer = 200;


	auto selector = canvas.addElement<gl::BoxSelector>("Selector", glm::vec2(cam->ScreenWidth, cam->ScreenHeight) * 0.5f, glm::vec2(100, 300));
	selector->layer = 1000;

	renderer.addRenderHook(gl::Renderer::RenderHook::Pre2DGui, [&](gl::Renderer* env) {
		canvas.handleEvents();
		canvas.draw();
	});

	renderer.addUIWindow("Canvas", [&](gl::Renderer* env) {
		ImGuiIO io =ImGui::GetIO();
		int x = io.MousePos.x;
		int y = io.MousePos.y;
		auto spritesUnderCursor = canvas.getIntersectingElements(x, y, false, true);
		ImGui::ColorEdit3("Lena color", &sprite2->color.x);
		ImGui::Text("Mouse Position %d %d", x, y);
		ImGui::Text("Selector: [(%.1f %.1f) - (%.1f %.1f)]",
			selector->position().x, selector->position().y,
			selector->size().x, selector->size().y);
		ImGui::Text("Sprites under the cursor:");
		for (auto sprite : spritesUnderCursor) {
			ImGui::Text("%s (Layer: %d)", sprite->name.c_str(), sprite->layer);
		}

	});

	while (!renderer.shouldClose()) {
		renderer.startFrame();
		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}