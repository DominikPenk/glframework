#pragma once
#include <functional>
#include <OpenMesh/Core/Mesh/Handles.hh>

namespace OpenMesh {
struct DebuggalbeMixin {

	DebuggalbeMixin() {
		debugBreakCallback_ = []() {
			__debugbreak();
		};
	}

	std::function<void()> debugBreakCallback_;

	inline bool shouldBreak(VertexHandle vh) const;
	inline bool shouldBreak(EdgeHandle eh) const;
	inline bool shouldBreak(HalfedgeHandle heh) const;
	inline bool shouldBreak(FaceHandle fh) const;

	inline void watch(VertexHandle vh);
	inline void watch(EdgeHandle eh);
	inline void watch(HalfedgeHandle heh);
	inline void watch(FaceHandle fh);


	inline void stopWatch(VertexHandle vh);
	inline void stopWatch(EdgeHandle eh);
	inline void stopWatch(HalfedgeHandle heh);
	inline void stopWatch(FaceHandle fh);

	std::vector<VertexHandle> watchedVertexHandles_;
	std::vector<EdgeHandle> watchedEdgeHandles_;
	std::vector<HalfedgeHandle> watchedHalfedgeHandles_;
	std::vector<FaceHandle> watchedFaceHandles_;
};

inline bool DebuggalbeMixin::shouldBreak(VertexHandle vh) const
{
	return std::find(watchedVertexHandles_.begin(), watchedVertexHandles_.end(), vh) != watchedVertexHandles_.end();
}
inline bool DebuggalbeMixin::shouldBreak(EdgeHandle eh) const
{
	return std::find(watchedEdgeHandles_.begin(), watchedEdgeHandles_.end(), eh) != watchedEdgeHandles_.end();
}
inline bool DebuggalbeMixin::shouldBreak(HalfedgeHandle heh) const
{
	return std::find(watchedHalfedgeHandles_.begin(), watchedHalfedgeHandles_.end(), heh) != watchedHalfedgeHandles_.end();
}
inline bool DebuggalbeMixin::shouldBreak(FaceHandle fh) const
{
	return std::find(watchedFaceHandles_.begin(), watchedFaceHandles_.end(), fh) != watchedFaceHandles_.end();
}

inline void DebuggalbeMixin::watch(VertexHandle vh)
{
	if (std::find(watchedVertexHandles_.begin(), watchedVertexHandles_.end(), vh) == watchedVertexHandles_.end()) {
		watchedVertexHandles_.push_back(vh);
	}
}

inline void DebuggalbeMixin::watch(EdgeHandle eh)
{
	if (std::find(watchedEdgeHandles_.begin(), watchedEdgeHandles_.end(), eh) == watchedEdgeHandles_.end()) {
		watchedEdgeHandles_.push_back(eh);
	}
}

inline void DebuggalbeMixin::watch(HalfedgeHandle heh)
{
	if (std::find(watchedHalfedgeHandles_.begin(), watchedHalfedgeHandles_.end(), heh) == watchedHalfedgeHandles_.end()) {
		watchedHalfedgeHandles_.push_back(heh);
	}
}

inline void DebuggalbeMixin::watch(FaceHandle fh)
{
	if (std::find(watchedFaceHandles_.begin(), watchedFaceHandles_.end(), fh) == watchedFaceHandles_.end()) {
		watchedFaceHandles_.push_back(fh);
	}
}

inline void DebuggalbeMixin::stopWatch(VertexHandle vh) {
	auto it = std::find(watchedVertexHandles_.begin(), watchedVertexHandles_.end(), vh);
	if (it == watchedVertexHandles_.end()) throw std::runtime_error("Tried to end watch on vertex handle that is not beeing watched");
	watchedVertexHandles_.erase(it);
}
inline void DebuggalbeMixin::stopWatch(EdgeHandle eh) {
	auto it = std::find(watchedEdgeHandles_.begin(), watchedEdgeHandles_.end(), eh);
	if (it == watchedEdgeHandles_.end()) throw std::runtime_error("Tried to end watch on edge handle that is not beeing watched");
	watchedEdgeHandles_.erase(it);
}
inline void DebuggalbeMixin::stopWatch(HalfedgeHandle heh) {
	auto it = std::find(watchedHalfedgeHandles_.begin(), watchedHalfedgeHandles_.end(), heh);
	if (it == watchedHalfedgeHandles_.end()) throw std::runtime_error("Tried to end watch on halfedge handle that is not beeing watched");
	watchedHalfedgeHandles_.erase(it);
}
inline void DebuggalbeMixin::stopWatch(FaceHandle fh) {
	auto it = std::find(watchedFaceHandles_.begin(), watchedFaceHandles_.end(), fh);
	if (it == watchedFaceHandles_.end()) throw std::runtime_error("Tried to end watch on face handle that is not beeing watched");
	watchedFaceHandles_.erase(it);
}
}
