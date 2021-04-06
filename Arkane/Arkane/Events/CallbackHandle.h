#pragma once

#include "../Core/Defines.h"

#include "Event.h"

#include <functional>
#include <list>
#include <map>
#include <optional>


AK_NAMESPACE_BEGIN

class CallbackHandle
{
	friend class EventManager;

public:
	CallbackHandle() = default;

	AK_INLINE bool IsValid() const { return m_iter.has_value(); }
	AK_INLINE EventId GetEventId() const { return m_eventId; }

private:
	typedef std::list<std::function<void(Event&)>>::iterator Iterator;

	CallbackHandle(EventId _eventId, Iterator _iter) : m_eventId(_eventId), m_iter(_iter) {}

	EventId m_eventId;
	std::optional<Iterator> m_iter;
};

AK_NAMESPACE_END