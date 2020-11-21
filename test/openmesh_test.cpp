#include <glpp/renderer.hpp>
#include <glpp/camera.hpp>
#include <glpp/controls.hpp>
#include <glpp/meshes.hpp>
#include <glpp/texture.hpp>
#include <glpp/framebuffer.hpp>
#include <glpp/intermediate.h>

#include <iostream>


int main(int argc, const char* argv[]) {
	auto cam = std::make_shared<gl::Camera>(
		glm::vec3(0.0f, 5.0f, 6.0f),
		glm::vec3(0, 0, 0));
	cam->Far = -1.f;
	cam->Near = 0.01f;
	gl::FlyingControl control(cam);

	gl::Renderer renderer(1024, 720, cam, "Test Window", false);
	renderer.showOutliner = false;

	//gl::TriangleMesh mesh(std::string(TEST_DIR) + "teapot.obj");
	auto mesh = renderer.addMesh<gl::OpenMeshMesh>("Teapot", std::string(TEST_DIR) + "teapot.obj");

	// Create a frame buffer to draw to
	gl::Framebuffer buffer(cam->ScreenWidth, cam->ScreenHeight);
	buffer.setRenderTexture(0, nullptr);

	// Show offscreen rendering result using intermediate rendering
	renderer.addRenderHook(gl::Renderer::RenderHook::Pre2DGui, [&](gl::Renderer* env) {
		gl::displayTexture(0, 0, cam->ScreenWidth * 0.25, cam->ScreenHeight * 0.25, buffer.getRenderTexture(0));
		});

	renderer.pushResizeCallback([&](gl::Renderer* env) {
		buffer.resize(cam->ScreenWidth, cam->ScreenHeight);
		});


	while (!renderer.shouldClose()) {
		renderer.startFrame();
		control.update(cam);

		// Render teapot offscreen
		buffer.bind();
		buffer.clear({ glm::vec4(0, 0, 0, 1) });
		mesh->render(&renderer);
		buffer.unbind();

		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}