#pragma once

#include <glm/glm.hpp>
#include "glpp/2d/canvas.hpp"

#include "glpp/2d/eventsystem.hpp"

namespace gl {

	class DrawBatch;


	class Handle : public EventReceiver {
	public:
		Handle();

		virtual void createGeometry(DrawBatch& batch) = 0;
		virtual bool overlaps(int x, int y) const = 0;

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

		virtual gl::EventState onMouseDown(float x, float y) override;
		virtual gl::EventState onDrag(float dx, float dy) override;
			
		glm::vec4 color;
		glm::vec2 position;
		glm::vec2 size;

	private:
		bool mIsActive;
	};

}