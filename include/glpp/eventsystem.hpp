#pragma once
#include <functional>
#include <string>
#include <unordered_map>

namespace gl {
	class Event {
	public:
		typedef std::string DescriptorType;
		virtual gl::Event::DescriptorType type() const = 0;
	};

	class SignalEvent : public Event {
	public:
		SignalEvent(gl::Event::DescriptorType name);
			
		virtual DescriptorType type() const override;

	private:
		Event::DescriptorType mName;

	};

	class EventSystem {
	public:
		EventSystem(); 
		typedef std::function<void(const Event&)> Slot;

		template<typename EventType, typename ...Args>
		void raise(Args... args) const {
			static_assert(std::is_base_of_v < gl::Event, EventType, "EventType must derive from gl::Event");
			Event e(args...);
			raise(e);
		}

		void raise(const Event& e) const;
		
		void signal(gl::Event::DescriptorType name) const;

		// Listen to all events
		size_t listen(const Slot&& callback);

		// Listen to a specific event
		size_t listen(gl::Event::DescriptorType type, const Slot&& callback);

		void stopListening(size_t listener);

	private:
		typedef std::unordered_map<size_t, Slot> ListenerDict;
		
		size_t mListenerId;

		// Store all listeners mapped to their triggering event
		std::unordered_map<Event::DescriptorType, ListenerDict> mObservers;

		// Maps listenerIds to triggering events
		std::unordered_map<size_t, std::vector<Event::DescriptorType>> mTriggerMap;
	};

}