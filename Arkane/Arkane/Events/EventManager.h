#pragma once

#include "Event.h"

#include <functional>
#include <list>
#include <unordered_map>


AK_NAMESPACE_BEGIN

class EventManager
{
public:
	static EventManager& GetInstance();

	void AddListener(EventId _eventId, std::function<void(Event&)> const& _listener)
	{
		m_listeners[_eventId].push_back(_listener);
	}

	void SendEvent(Event& _event);
	void SendEvent(EventId _eventId);

private:
	std::unordered_map<EventId, std::list<std::function<void(Event&)>>> m_listeners;
};


AK_NAMESPACE_END


#define akAddMethodEventListener(_event, _callback) Arkane::EventManager::GetInstance().AddListener(AK_EVENT_METHOD_LISTENER(_event, _callback))
#define akAddFunctionEventListener(_event, _callback) Arkane::EventManager::GetInstance().AddListener(AK_EVENT_FUNCTION_LISTENER(_event, _callback))
#define akSendEvent(_event) Arkane::EventManager::GetInstance().SendEvent(_event)
#define akSendEventId(_event) Arkane::EventManager::GetInstance().SendEvent(_event)