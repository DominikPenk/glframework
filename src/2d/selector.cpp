#include "glpp/2d/selector.hpp"

#include "glpp/2d/eventsystem.hpp"

gl::BoxSelector::BoxSelector(glm::vec2 position, glm::vec2 size) :
	CanvasElement(),
	mHandleSize(10),
	mDraggedHandle(-1),
	color(233.f / 255.f, 196.f / 255.f, 106.f / 255.f, 1.f),
	mUpdated(true)
{
	// TODO get the correct shader
	mShader = std::make_shared<gl::Shader>(std::string(GL_FRAMEWORK_SHADER_DIR) + "2d_handles.glsl");
	mBatch.addVertexAttributes<glm::vec2, glm::vec4>(0);

	// Corner Handles
	mHandles.push_back(BoxHandle(position + glm::vec2(-0.5f, -0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2( 0.5f, -0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2( 0.5f,  0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2(-0.5f,  0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));

	// Edge Handles
	mHandles.push_back(BoxHandle(position + glm::vec2(-0.5f,  0.0f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2( 0.5f,  0.0f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2( 0.0f, -0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2( 0.0f,  0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));

	mHandles.push_back(BoxHandle(position, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position, size, glm::vec4(1, 1, 1, 0.25f)));
	mHandles.back().ignoreInteractions = true;
}

void gl::BoxSelector::draw(int width, int height, int layers)
{
	if (mDraggedHandle != -1) {
		// Update all handles
		glm::vec2 min, max;
		switch(mDraggedHandle) {
		case 0:
			min = mHandles[0].position;
			max = mHandles[2].position;
			break;
		case 2:
			min = mHandles[0].position;
			max = mHandles[2].position;
			break;
		case 1:
			min = glm::vec2(mHandles[0].position.x, mHandles[1].position.y);
			max = glm::vec2(mHandles[1].position.x, mHandles[2].position.y);
			break;
		case 3:
			min = glm::vec2(mHandles[3].position.x, mHandles[0].position.y);
			max = glm::vec2(mHandles[2].position.x, mHandles[3].position.y);
			break;
		case 4:
			min = glm::vec2(mHandles[4].position.x, mHandles[0].position.y);
			max = mHandles[2].position;
			break;
		case 5:
			min = mHandles[0].position;
			max = glm::vec2(mHandles[5].position.x, mHandles[2].position.y);
			break;
		case 6:
			min = glm::vec2(mHandles[0].position.x, mHandles[6].position.y);
			max = mHandles[2].position;
			break;
		case 7:
			min = mHandles[0].position;
			max = glm::vec2(mHandles[2].position.x, mHandles[7].position.y);
			break;
		case 8:
			min = mHandles[8].position - 0.5f * mHandles[9].size;
			max = mHandles[8].position + 0.5f * mHandles[9].size;
		}

		// Corners
		mHandles[0].position = min;
		mHandles[1].position = glm::vec2(max.x, min.y);
		mHandles[2].position = max;
		mHandles[3].position = glm::vec2(min.x, max.y);
		
		// Edges
		glm::vec2 pos = 0.5f * (min + max);
		mHandles[4].position = glm::vec2(min.x, pos.y);
		mHandles[5].position = glm::vec2(max.x, pos.y);
		mHandles[6].position = glm::vec2(pos.x, min.y);
		mHandles[7].position = glm::vec2(pos.x, max.y);

		// Center
		mHandles[8].position = pos;
		mHandles[9].position = pos;
		mHandles[9].size = glm::abs(max - min);
		
		mUpdated = true;
	}

	if (mUpdated) {
		mBatch.clear();
		for (BoxHandle& handle : mHandles) {
			handle.createGeometry(mBatch);
		}
		mUpdated = false;
	}
	
	float zOrder = 1.0f - 2.0f * std::clamp(layer, 1, layers - 1) / layers;

	mBatch.execute(*mShader,
		"canvasSize", glm::vec2(width, height),
		"hue", color,
		"z", zOrder);
}

bool gl::BoxSelector::overlaps(float x, float y) const
{
	bool ret = false;
	for (const gl::BoxHandle& h : mHandles) {
		ret = ret || h.overlaps(x, y);
		if (ret) 
		{
			return true;
		}
	}
	return false;
}

gl::EventState gl::BoxSelector::onMouseDown(float x, float y)
{
	for (int i = 0; i < (int)mHandles.size(); ++i) {
		if (mHandles[i].overlaps(x, y) && mHandles[i].onMouseDown(x, y) == EventState::StartDrag) {
			mDraggedHandle = i;
			return EventState::StartDrag;
		}
	}
	return EventState::Pass;
}

gl::EventState gl::BoxSelector::onDrag(float dx, float dy)
{
	assert(mDraggedHandle >= 0);
	mUpdated = true;
	return mHandles[mDraggedHandle].onDrag(dx, dy);
}

gl::EventState gl::BoxSelector::onDragEnd(float x, float y)
{
	mDraggedHandle = -1;
	glm::vec2 min = glm::min(mHandles[0].position, glm::min(mHandles[1].position, glm::min(mHandles[2].position, mHandles[3].position)));
	glm::vec2 max = glm::max(mHandles[0].position, glm::max(mHandles[1].position, glm::max(mHandles[2].position, mHandles[3].position)));
	
	glm::vec2 position = 0.5f * (min + max);
	glm::vec2 size = max - min;
	update(position, size);

	return EventState::Stop;
}

void gl::BoxSelector::update(glm::vec2 position, glm::vec2 size)
{
	// center
	mHandles[8].position = mHandles[8].position = position;
	mHandles[9].size = size;

	// Corners
	mHandles[0].position = position + glm::vec2(-0.5f, -0.5f) * size;
	mHandles[1].position = position + glm::vec2( 0.5f, -0.5f) * size;
	mHandles[2].position = position + glm::vec2( 0.5f,  0.5f) * size;
	mHandles[3].position = position + glm::vec2(-0.5f,  0.5f) * size;
	
	// Edges
	mHandles[4].position = position + glm::vec2(-0.5f,  0.0f) * size;
	mHandles[5].position = position + glm::vec2( 0.5f,  0.0f) * size;
	mHandles[6].position = position + glm::vec2( 0.0f, -0.5f) * size;
	mHandles[7].position = position + glm::vec2( 0.0f,  0.5f) * size;
	
	mUpdated = true;
}

glm::vec2 gl::BoxSelector::position() const
{
	return mHandles.back().position;
}

glm::vec2 gl::BoxSelector::size() const
{
	return mHandles.back().size;
}
