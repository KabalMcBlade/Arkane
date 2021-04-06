#pragma once

#include "../Core/Defines.h"
#include "../Utilities/Hash.h"
#include "../ECS/Core/Types.h"

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

	explicit Event(EventId _type) : m_type(_type), m_entity(InvalidEntity), m_data({}) {}
	explicit Event(EventId _type, Entity _entity) : m_type(_type), m_entity(_entity), m_data({}) {}

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

	bool HasSpecificEntity() const
	{
		return m_entity != InvalidEntity;
	}

	Entity GetEntity() const
	{
		return m_entity;
	}

private:
	Event();

private:
	EventId m_type;
	Entity m_entity;
	std::unordered_map<EventId, std::any> m_data;
};

AK_NAMESPACE_END
