#include <glpp/renderers/offscreen_renderer.hpp>

#include <glpp/context.hpp>
#include <glpp/framebuffer.hpp>

#include <stdexcept>

gl::OffscreenRenderer::OffscreenRenderer(int width, int height) :
	mContext(nullptr),
	framebuffer(nullptr)
{
	mContext = std::make_shared<gl::OffscreenContext>(width, height);
	framebuffer = std::make_shared<gl::Framebuffer>(width, height);
	clearColors = { {0, 0, 0, 1} };
}

void gl::OffscreenRenderer::startRender(int width, int height, bool clear)
{
	mContext->makeCurrent();
	if (framebuffer != nullptr) {
		if (framebuffer->width() != width || framebuffer->height() != height) {
			framebuffer->resize(width, height);
		}
		if (clear) {
			if (framebuffer->numColorAttachments() != clearColors.size()) {
				throw std::runtime_error("Different number of color attachments and clear colors");
			}
			framebuffer->clearDepthAndStencilBuffer();
			for (size_t i = 0; i < clearColors.size(); ++i) {
				framebuffer->clearColorAttachment((int)i, clearColors[i]);
			}
		}
		framebuffer->bind();
	}
}
