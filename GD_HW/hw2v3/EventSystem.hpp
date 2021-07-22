#ifndef GD_EVENT_SYSTEM
#define GD_EVENT_SYSTEM

#include <vector>

template<class Event>
class EventObserver;

template<class Event>
class EventSubject;


template<class Event>
class EventObserver {
	friend class EventSubject<Event>;

private:
	virtual bool onEvent(Event event, void *caller, void *eventData) { return false; }
};


template<class Event>
class EventSubject {
public:
	/// register a new observer to receive events
	virtual bool addObserver(EventObserver<Event> *observer) {
		this->_observers.push_back(observer);
		return true;
	}

	/// unregister an observer
	virtual bool removeObserver(const EventObserver<Event> *observer) {
		for (auto it = _observers.begin(); it != _observers.end(); it++) {
			if (*it == observer) {
				this->_observers.erase(it);
				return true;
			}
		}
		return false;
	}

protected:
	/// notify all observers
	virtual bool triggerEvent(Event event, void *caller, void *eventData) {
		bool result = false;
		for (auto &_observer : _observers) {
			result |= _observer->onEvent(event, caller, eventData);
		}
		return result;
	}

	/// only notify observers till one returns true
	virtual bool triggerSingleEvent(Event event, void *caller, void *eventData) {
		for (auto &_observer : _observers) {
			bool result = _observer->onEvent(event, caller, eventData);
			if (result) return true;
		}
		return false;
	}

	/// unregister all observers
	virtual bool clearObservers() {
		this->_observers.clear();
		return true;
	}

protected:
	std::vector<EventObserver<Event> *> _observers;
};

#endif