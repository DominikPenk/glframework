#include "glpp/2d/selector.hpp"

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

	// Center
	mHandles.push_back(BoxHandle(position + glm::vec2(-0.5f, -0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2( 0.5f, -0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2( 0.5f,  0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));
	mHandles.push_back(BoxHandle(position + glm::vec2(-0.5f,  0.5f) * size, glm::vec2(mHandleSize), glm::vec4(1, 1, 1, 1)));

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
			min = mHandles[4].position - 0.5f * mHandles[5].size;
			max = mHandles[4].position + 0.5f * mHandles[5].size;
		}

		mHandles[0].position = min;
		mHandles[1].position = glm::vec2(max.x, min.y);
		mHandles[2].position = max;
		mHandles[3].position = glm::vec2(min.x, max.y);
		mHandles[4].position = 0.5f * (min + max);
		mHandles[5].position = 0.5f * (min + max);
		mHandles[5].size = glm::abs(max - min);
		
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

bool gl::BoxSelector::overlaps(int x, int y) const
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

int gl::BoxSelector::onMouseDown(int x, int y)
{
	for (int i = 0; i < (int)mHandles.size(); ++i) {
		if (mHandles[i].overlaps(x, y) && mHandles[i].onMouseDown(x, y) == CanvasElement::START_DRAG) {
			mDraggedHandle = i;
			return CanvasElement::START_DRAG;
		}
	}
	return CanvasElement::PASS;
}

int gl::BoxSelector::onMouseDrag(int dx, int dy)
{
	assert(mDraggedHandle != -1);
	return mHandles[mDraggedHandle].onMouseDrag(dx, dy);
}

int gl::BoxSelector::onMouseUp(int xx, int xy)
{
	mDraggedHandle = -1;
	return CanvasElement::STOP;
}

void gl::BoxSelector::update(glm::vec2 position, glm::vec2 size)
{
	mHandles[4].position = mHandles[5].position = position;
	mHandles[5].size = size;

	mHandles[0].position = position + glm::vec2(-0.5f, -0.5f) * size;
	mHandles[1].position = position + glm::vec2( 0.5f, -0.5f) * size;
	mHandles[2].position = position + glm::vec2( 0.5f,  0.5f) * size;
	mHandles[3].position = position + glm::vec2(-0.5f,  0.5f) * size;
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
