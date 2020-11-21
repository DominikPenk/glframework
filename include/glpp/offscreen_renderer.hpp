#include "glpp/renderer.hpp"


namespace gl {
	class OffscreenRenderer : public RendererBase {
	public:
		OffscreenRenderer(std::shared_ptr<Camera> cam);
		~OffscreenRenderer();
		
		void startRender(size_t width, size_t height);
	};
}