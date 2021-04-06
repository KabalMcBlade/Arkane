#pragma once

#include "Event.h"
#include "CallbackHandle.h"

#include <functional>
#include <list>
#include <unordered_map>


AK_NAMESPACE_BEGIN

class EventManager
{
public:
	static EventManager& GetInstance();

	CallbackHandle AddListener(EventId _eventId, std::function<void(Event&)> const& _listener)
	{
		return CallbackHandle(_eventId, m_listeners[_eventId].insert(m_listeners[_eventId].begin(), _listener));
	}

	bool RemoveListener(CallbackHandle _handle)
	{
		if (_handle.IsValid())
		{
			if (m_listeners.find(_handle.GetEventId()) != m_listeners.end())
			{
				return m_listeners[_handle.GetEventId()].erase(_handle.m_iter.value()) != m_listeners[_handle.GetEventId()].end();
			}
		}

		return false;
	}

	void SendEvent(Event& _event);
	void SendEvent(EventId _eventId);

private:
	std::unordered_map<EventId, std::list<std::function<void(Event&)>>> m_listeners;
};


AK_NAMESPACE_END


#define akAddMethodEventListener(_event, _callback) Arkane::EventManager::GetInstance().AddListener(AK_EVENT_METHOD_LISTENER(_event, _callback))
#define akAddFunctionEventListener(_event, _callback) Arkane::EventManager::GetInstance().AddListener(AK_EVENT_FUNCTION_LISTENER(_event, _callback))
#define akRemoveMethodEventListener(_handle) Arkane::EventManager::GetInstance().RemoveListener(_handle)
#define akRemoveFunctionEventListener(_handle) Arkane::EventManager::GetInstance().RemoveListener(_handle)
#define akSendEvent(_event) Arkane::EventManager::GetInstance().SendEvent(_event)
#define akSendEventId(_event) Arkane::EventManager::GetInstance().SendEvent(_event)