#pragma once

#include "glpp/2d/canvas.hpp"
#include "glpp/2d/handles.hpp"
#include "glpp/draw_batch.hpp"

namespace gl {

	class BoxSelector : public CanvasElement {
	public:
		BoxSelector(glm::vec2 position, glm::vec2 size);

		virtual void draw(int width, int height, int layers);

		virtual bool overlaps(int x, int y) const;

		virtual int onMouseDown(int x, int y); 
		virtual int onMouseDrag(int dx, int dy);
		virtual int onMouseUp(int xx, int xy);

		glm::vec2 position() const;
		glm::vec2 size() const;
		glm::vec4 color;

	private:
		int mHandleSize;
		int mDraggedHandle;
		gl::DrawBatch mBatch;
		std::shared_ptr<gl::Shader> mShader;
		std::vector<gl::BoxHandle> mHandles;

	};

}