#include "eventsystem.hpp"

#include <glpp/logging.hpp>

gl::SignalEvent::SignalEvent(gl::Event::DescriptorType name) :
	mName(name)
{
}

gl::Event::DescriptorType gl::SignalEvent::type() const
{
	return mName;
}


gl::EventSystem::EventSystem() :
	mListenerId(0)
{
}

void gl::EventSystem::raise(const Event& e) const
{
	auto t = e.type();

	LOG("Raising event \"%s\"", t.c_str());

	if (mObservers.find(t) == mObservers.end()) {
		return;
	}

	auto&& observers = mObservers.at(t);

	for (auto&& [_, cb] : observers) {
		cb(e);
	}
}

void gl::EventSystem::signal(gl::Event::DescriptorType name) const
{
	raise(SignalEvent(name));
}

size_t gl::EventSystem::listen(const Slot&& callback)
{
	mTriggerMap[mListenerId] = { "__ALL__" };
	mObservers["__ALL__"][mListenerId] = callback;
	return mListenerId++;
}

size_t gl::EventSystem::listen(gl::Event::DescriptorType type, const Slot&& callback)
{
	LOG_WARNING_IF(type == "__ALL__", "You should directly listen to the __ALL__ event. Use EventSysten::listen(<callback>) instead.");
	mTriggerMap[mListenerId] = { type };
	mObservers[type][mListenerId] = callback;
	return mListenerId++;
}

void gl::EventSystem::stopListening(size_t listener)
{
	if (mTriggerMap.find(listener) == mTriggerMap.end()) {
		LOG_WARNING("Tried to remove listener %ul which was not found", listener);
		return;
	}

	for (auto t : mTriggerMap.at(listener)) {
		mObservers[t].erase(listener);
	}
}

