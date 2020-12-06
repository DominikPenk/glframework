#pragma once

#include <glm/glm.hpp>
#include "glpp/2d/canvas.hpp"

#include "glpp/2d/eventsystem.hpp"

namespace gl {

	class DrawBatch;

	class Handle : public EventReceiver {
	public:
		enum Constraint {
			XAxis,
			YAxsis,
			None
		};
		
		Handle();
		Handle(glm::vec2 position, glm::vec4 color);

		virtual void createGeometry(DrawBatch& batch) = 0;
		virtual bool overlaps(int x, int y) const = 0;

		virtual gl::EventState onMouseDown(float x, float y) override;
		virtual gl::EventState onDrag(float dx, float dy) override;
		
		bool ignoreInteractions;
		glm::vec4 color;
		glm::vec2 position;
		Constraint constrain;

	protected:
	};

	class BoxHandle : public Handle {
	public:
		BoxHandle(glm::vec2 position, glm::vec2 size, glm::vec4 color);

		virtual void createGeometry(DrawBatch& batch) override;
		virtual bool overlaps(int x, int y) const override;

		glm::vec2 size;
	};

}