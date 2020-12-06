#pragma once

#include <tuple>

namespace gl {

	class Canvas;

	enum class EventState {
		Pass,
		Stop,
		StartDrag,
		StopDrag
	};

	class EventReceiver {
	public:
		virtual gl::EventState onMouseDown(float x, float y);
		virtual gl::EventState onMouseUp(float x, float y);

		virtual gl::EventState onDrag(float dx, float dy);
		virtual gl::EventState onDragEnd(float x, float y);

		// Normally you will not need to override the following functions.
		// They wrap the onXXX functions to also return the self reference for the event system
		// You will need to override this if your class consists of a collection of EventReceivers and return the actual receiver that triggers
		virtual std::pair<gl::EventState, gl::EventReceiver*> _onMouseDown(float x, float y);
	};

	class EventSystem {
	public:
		EventSystem();
		void handleEvents(gl::Canvas& canvas);

	protected:
		EventReceiver* mActiveReceiver;
		bool           mDragging;
	};

}