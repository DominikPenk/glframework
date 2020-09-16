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

	renderer.startRender(1024, 720);
	buffer.clearColorAttachment(1, -1);
	CImg<int> image((int)cam->ScreenWidth, (int)cam->ScreenHeight, 1);
	std::printf("Generated image of size: %dx%d with %d channels\n", image.height(), image.width(), image.depth());
	std::printf("Camera: %dx%d\n", (int)cam->ScreenHeight, (int)cam->ScreenWidth);
	buffer.bind();
	buffer.clear({ glm::vec4(0, 0, 0, 1) });
	mesh.render(&renderer);
	buffer.readColorAttachment(1, 0, 0, cam->ScreenWidth, cam->ScreenHeight, (void*)image.data());
	buffer.unbind();

	// Convert to uchar
	CImg<unsigned char> disp_img(image.width(), image.height(), 1, 3, 0);
	for (int j = 0; j < image.height(); ++j) {
		for (int i = 0; i < image.width(); ++i) {
			float t = image(i, image.height() - 1 - j) / 2463.0;
			if (t < 0) {
				continue;
			}
			disp_img(i, image.height() - 1 - j, 0) = 255;
			disp_img(i, image.height() - 1 - j, 1) = 255 * (1.0f - t);
			disp_img(i, image.height() - 1 - j, 2) = 255 * (1.0f - t);
		}	
	}

	CImgDisplay main_disp(disp_img, "Offscreen Result");
	while (!main_disp.is_closed()) {
		main_disp.wait();
	}


	return EXIT_SUCCESS;
}