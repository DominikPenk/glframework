#include "CImg.h"

#include <glpp/renderers/offscreen_renderer.hpp>
#include <glpp/camera.hpp>
#include <glpp/controls.hpp>
#include <glpp/meshes.hpp>
#include <glpp/texture.hpp>

#include <iostream>

using namespace cimg_library;


int main() {
	// Set up renderer
	auto cam = std::make_shared<gl::Camera>(
		glm::vec3(0.0f, 4.0f, 10.0f),
		glm::vec3(0, 0, 0));
	cam->Far = -1.f;
	cam->Near = 0.01f;
	cam->ScreenHeight = 720;
	cam->ScreenWidth = 1024;

	//gl::Renderer renderer(1024, 720, cam, "Test Window", false);
	gl::OffscreenRenderer renderer(cam->ScreenHeight, cam->ScreenWidth);

	// Create a triangle
	gl::TriangleMesh teapot(std::string(SAMPLE_DIR) + "teapot.obj");
	teapot.visualizeNormals = true;

	glEnable(GL_DEPTH_TEST);

	auto render_offscreen = [&](int frame) {
		renderer.startRender(1024, 720);

		// Update camera position
		float t = (frame % 120) / 120.f;
		float angle = 6.28318530718 * t;
		float radius = 7.0f;
		glm::vec3 pos(radius * std::cos(angle), 6.0f, radius * std::sin(angle));
		cam->lookAt(glm::vec3(0), pos);
		
		std::vector<unsigned char> data(4 * cam->ScreenWidth * cam->ScreenHeight);
		teapot.render(cam);
		renderer.framebuffer->readColorAttachment(0, 0, 0, cam->ScreenWidth, cam->ScreenHeight, (void*)data.data());

		// Convert to uchar
		CImg<unsigned char> img(cam->ScreenWidth, cam->ScreenHeight, 1, 3, 255);
		for (int j = 0; j < img.height(); ++j) {
			for (int i = 0; i < img.width(); ++i) {
				int idx = 4 * (j * cam->ScreenWidth + i);
				img(i, img.height() - j - 1, 0) = data[idx];
				img(i, img.height() - j - 1, 1) = data[idx + 1];
				img(i, img.height() - j - 1, 2) = data[idx + 2];
			}
		}
		return img;
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