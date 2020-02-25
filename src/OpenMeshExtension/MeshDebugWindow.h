#pragma once

#include <condition_variable>

#include "../uiwindow.hpp"
#include "openmesh_mesh.h"
#include "DebuggableMixin.h"

namespace gl {

	class MeshDebugWindow : public UIWindow {
	public:
		MeshDebugWindow();
		virtual void onDraw(Renderer* env) override;

	protected:
		std::function<void()> mBreakPointCallback;

		std::condition_variable breakpoint;
		std::mutex mutex;
		bool breakPointTriggered;
	};

}