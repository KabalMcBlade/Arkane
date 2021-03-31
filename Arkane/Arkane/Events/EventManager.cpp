#include "EventManager.h"


AK_NAMESPACE_BEGIN

EventManager& EventManager::GetInstance()
{
	static EventManager instance;
	return instance;
}


void EventManager::SendEvent(Event& _event)
{
	uint32_t type = _event.GetType();

	for (auto const& listener : m_listeners[type])
	{
		listener(_event);
	}
}

void EventManager::SendEvent(EventId _eventId)
{
	Event event(_eventId);

	for (auto const& listener : m_listeners[_eventId])
	{
		listener(event);
	}
}

AK_NAMESPACE_END