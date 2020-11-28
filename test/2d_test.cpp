#include <glpp/2d/canvas.hpp>
#include <glpp/2d/sprite.hpp>


#include <glpp/renderer.hpp>

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

	auto sprite1 = canvas.addSprite<gl::Sprite>("Block");
	sprite1->color = glm::vec3(1, 0, 0);
	sprite1->size = glm::vec2(250, 125);
	sprite1->position = glm::vec2(100, 500);
	sprite1->layer = 100;

	auto sprite2 = canvas.addSprite<gl::Sprite>("Image", std::string(TEST_DIR) + "lena.jpg");
	sprite2->position = glm::vec2(100, 0);


	renderer.addRenderHook(gl::Renderer::RenderHook::Pre2DGui, [&](gl::Renderer* env) {
		canvas.draw();
	});

	while (!renderer.shouldClose()) {
		renderer.startFrame();
		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}