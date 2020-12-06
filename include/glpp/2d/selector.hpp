#pragma once

#include "glpp/2d/canvas.hpp"
#include "glpp/2d/eventsystem.hpp"
#include "glpp/2d/handles.hpp"
#include "glpp/draw_batch.hpp"

#include <tuple>

namespace gl {

	class BoxSelector : public EventReceiver, public CanvasElement  {
	public:
		BoxSelector(glm::vec2 position, glm::vec2 size);

		virtual void draw(int width, int height, int layers);

		virtual bool overlaps(float x, float y) const;

		virtual gl::EventState onMouseDown(float x, float y) override;
		virtual gl::EventState onDrag(float dx, float dy) override;
		virtual gl::EventState onDragEnd(float x, float y) override;

		void update(glm::vec2 position, glm::vec2 size);

		glm::vec2 position() const;
		glm::vec2 size() const;
		glm::vec4 color;

	private:
		bool mUpdated;
		int mHandleSize;
		int mDraggedHandle;
		gl::DrawBatch mBatch;
		std::shared_ptr<gl::Shader> mShader;
		std::vector<gl::BoxHandle> mHandles;

	};

}