#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace gl {

	class OffscreenContext;
	class Framebuffer;
	
	class OffscreenRenderer {
	public:
		OffscreenRenderer(int width, int height);

		/// <summary>
		/// Prepares the bound framebuffer for a new render by resizing and clearing it
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void startRender(int width, int height, bool clear=true);

		std::shared_ptr<gl::Framebuffer> framebuffer;
		std::vector<glm::vec4> clearColors;
	protected:
		std::shared_ptr<gl::OffscreenContext> mContext;
	};
}