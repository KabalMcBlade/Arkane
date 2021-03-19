#pragma once

#include "../Core/Defines.h"
#include "../Utilities/Hash.h"

#include <any>
#include <unordered_map>


AK_NAMESPACE_BEGIN

using EventId = uint32_t;
using ParamId = uint32_t;

#define AK_EVENT_METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)
#define AK_EVENT_FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)


class Event final
{
public:
	~Event() { }

	explicit Event(EventId _type) : m_type(_type), m_data({}) {}

	template<typename T>
	void SetParam(EventId _id, T _value)
	{
		m_data[_id] = _value;
	}

	template<typename T>
	T GetParam(EventId _id)
	{
		return std::any_cast<T>(m_data[_id]);
	}

	EventId GetType() const
	{
		return m_type;
	}

private:
	Event();

private:
	EventId m_type;
	std::unordered_map<EventId, std::any> m_data;
};

AK_NAMESPACE_END
