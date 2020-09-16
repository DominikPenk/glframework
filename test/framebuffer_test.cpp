#include "renderer.hpp"
#include "camera.hpp"
#include "controls.hpp"
#include "splinecurves.hpp"
#include "openmesh_mesh.h"
#include "texture.hpp"

#include "framebuffer.hpp"

#include <iostream>

#include <OpenMesh/Core/IO/MeshIO.hh>


int main(int argc, const char* argv[]) {
	auto cam = std::make_shared<gl::Camera>(
		glm::vec3(3.0f, -5.0f, -2.0f),
		glm::vec3(0, 0, 0));
	cam->Far = 200;
	gl::OrbitControl control(cam);

	gl::Renderer renderer(1024, 720, cam, "Test Window", false);
	renderer.showOutliner = false;

	// Create a triangle
	OpenMesh::TriangleMesh3f teapot;
	if (!OpenMesh::IO::read_mesh(teapot, std::string(TEST_DIR) + "teapot.obj")) {
		std::cout << "Could not load teapot!\n";
		return EXIT_FAILURE;
	}
	gl::OpenMeshMesh mesh(teapot);
	mesh.getShader() = gl::Shader(std::string(TEST_DIR) + "test-shader.glsl");

	// Create a frame buffer to draw to
	gl::Framebuffer buffer(cam->ScreenWidth, cam->ScreenHeight);
	buffer.setRenderTexture(0, nullptr);

	// Generate an index texture
	std::shared_ptr<gl::Texture> index_texture = std::make_shared<gl::Texture>(
		cam->ScreenWidth, cam->ScreenHeight,
		GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT);
	index_texture->magFilterType = GL_NEAREST;
	index_texture->minFilterType = GL_NEAREST;
	index_texture->generateMipMap = false;
	buffer.appendRenderTexture(index_texture);

	// Window that will display the offscreen result
	renderer.addUIWindow("Offscreen Result", [&](gl::Renderer* env) {
		GLuint tid = buffer.getRenderTexture(0)->id;
		ImGui::Image((GLvoid*)tid, ImVec2(cam->ScreenWidth, cam->ScreenHeight));
		});

	renderer.pushResizeCallback([&](gl::Renderer* env) {
		buffer.resize(cam->ScreenWidth, cam->ScreenHeight);
		});


	// Render teapot offscreen
	buffer.bind();
	buffer.clear({ glm::vec4(0, 0, 0, 1) });
	mesh.render(&renderer);
	buffer.unbind();

	while (!renderer.shouldClose()) {
		renderer.startFrame();
		control.update(cam);

		renderer.endFrame();
	}

	return EXIT_SUCCESS;
}