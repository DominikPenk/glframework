#pragma once

#include <glm/glm.hpp>
#include "glpp/2d/canvas.hpp"

namespace gl {

	class DrawBatch;

	class Handle {
	public:
		Handle();

		virtual void createGeometry(DrawBatch& batch) = 0;
		virtual bool overlaps(int x, int y) const = 0;

		virtual int onMouseEnter(int x, int y) { return CanvasElement::PASS; }
		virtual int onMouseLeave(int x, int y) { return CanvasElement::PASS; }
		virtual int onMouseDrag(int deltaX, int deltaY) { return CanvasElement::PASS; }

		virtual int onMouseDown(int x, int y) { return CanvasElement::PASS; };
		virtual int onMouseUp(int x, int y) { return CanvasElement::PASS; };

		const int& hid;

		bool ignoreInteractions;

	protected:
		int mId;

	private:
		static int mNextId;
	};

	class BoxHandle : public Handle {
	public:
		BoxHandle(glm::vec2 position, glm::vec2 size, glm::vec4 color);

		virtual void createGeometry(DrawBatch& batch) override;
		virtual bool overlaps(int x, int y) const override;

		virtual int onMouseDown(int x, int y) override;
		virtual int onMouseDrag(int dx, int dy) override;
			
		glm::vec4 color;
		glm::vec2 position;
		glm::vec2 size;

	private:
		bool mIsActive;
	};

}