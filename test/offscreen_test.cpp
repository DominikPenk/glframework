#include "CImg.h"

#include "offscreen_renderer.hpp"
#include "camera.hpp"
#include "controls.hpp"
#include "splinecurves.hpp"
#include "openmesh_mesh.h"
#include "texture.hpp"

#include <iostream>

using namespace cimg_library;


int main() {
	// Set up renderer
	auto cam = std::make_shared<gl::Camera>(
		glm::vec3(3.0f, -5.0f, -2.0f),
		glm::vec3(0, 0, 0));
	cam->Far = 200;
	cam->ScreenHeight = 720;
	cam->ScreenWidth = 1024;

	//gl::Renderer renderer(1024, 720, cam, "Test Window", false);
	gl::OffscreenRenderer renderer(cam);

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
		GL_RED_INTEGER, GL_R32I, GL_INT);
	index_texture->magFilterType = GL_NEAREST;
	index_texture->minFilterType = GL_NEAREST;
	index_texture->generateMipMap = false;
	buffer.appendRenderTexture(index_texture);

	auto render_offscreen = [&](int frame) {
		renderer.startRender(1024, 720);

		// Update camera position
		float t = (frame % 120) / 120.f;
		float angle = 6.28318530718 * t;
		glm::vec3 pos(3.0f * std::cos(angle), 2, 3.0f * std::sin(angle));
		cam->lookAt(glm::vec3(0), pos);
		
		buffer.clearColorAttachment(1, -1);
		CImg<int> image((int)cam->ScreenWidth, (int)cam->ScreenHeight, 1);
		buffer.bind();
		buffer.clear({ glm::vec4(0, 0, 0, 1) });
		mesh.render(&renderer);
		buffer.readColorAttachment(1, 0, 0, cam->ScreenWidth, cam->ScreenHeight, (void*)image.data());
		buffer.unbind();
		CImg<unsigned char> disp_img(image.width(), image.height(), 1, 3, 0);

		// Convert to uchar
		for (int j = 0; j < image.height(); ++j) {
			for (int i = 0; i < image.width(); ++i) {
				float t = image(i, j) / 2463.0;
				if (t < 0) {
					continue;
				}
				disp_img(i, j, 0) = 255;
				disp_img(i, j, 1) = 255 * (1.0f - t);
				disp_img(i, j, 2) = 255 * (1.0f - t);
			}
		}
		return disp_img;
	};

	int frame = 0;
	CImg<unsigned char> disp_img = render_offscreen(frame++);
	CImgDisplay main_disp(disp_img, "Offscreen Result");
	while (!main_disp.is_closed()) {
		disp_img = render_offscreen(frame++);
		disp_img.display(main_disp);
	}


	return EXIT_SUCCESS;
}