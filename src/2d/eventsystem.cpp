#include "glpp/2d/eventsystem.hpp"

#include "glpp/imgui.hpp"
#include "glpp/2d/canvas.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

gl::EventState gl::EventReceiver::onMouseDown(float x, float y)
{
    return gl::EventState::Pass;
}

gl::EventState gl::EventReceiver::onMouseUp(float x, float y)
{
    return gl::EventState::Pass;
}

gl::EventState gl::EventReceiver::onDrag(float dx, float dy)
{
    return gl::EventState::Pass;
}

gl::EventState gl::EventReceiver::onDragEnd(float x, float y)
{
    return gl::EventState::Pass;
}

std::pair<gl::EventState, gl::EventReceiver*> gl::EventReceiver::_onMouseDown(float x, float y)
{
    gl::EventState state = onMouseDown(x, y);
    return { state, (state != gl::EventState::Pass ? this : nullptr) };
}

gl::EventSystem::EventSystem() :
    mActiveReceiver(nullptr),
    mDragging(false)
{
}

void gl::EventSystem::handleEvents(gl::Canvas& canvas)
{
	ImGuiIO io = ImGui::GetIO();
	int w, h;
	glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
	const int lx = io.MousePos.x - canvas.position.x;
	const int ly = h - io.MousePos.y - 1 - canvas.position.y;

	if (mActiveReceiver != nullptr && mDragging) {
		EventState state = EventState::Pass;
		if (!io.MouseDown[0]) {
			state = mActiveReceiver->onDragEnd(lx, ly);
			mActiveReceiver = nullptr;
		}
		else {
			state = mActiveReceiver->onDrag((int)io.MouseDelta.x, -(int)io.MouseDelta.y);
		}
		if (state != EventState::Pass) {
			return;
		}
	}
	else if (io.MouseDown[0]) {
		auto elements = canvas.getIntersectingElements(lx, ly);
		for (auto element : elements) {
			auto receiver = std::dynamic_pointer_cast<gl::EventReceiver>(element);
			if (receiver == nullptr) {
				return;
			}
			else
			{
				EventState state;
				std::tie(state, mActiveReceiver) = receiver->_onMouseDown(lx, ly);
				if (state == EventState::StartDrag) {
					mDragging = true;
					break;
				}
				else if (state == EventState::Stop) {
					break;
				}
			}
		}
	}
	else if (!io.MouseDown[0] && mActiveReceiver != nullptr) {
		mActiveReceiver->onMouseUp(lx, ly);
		mActiveReceiver = nullptr;
	}
}
