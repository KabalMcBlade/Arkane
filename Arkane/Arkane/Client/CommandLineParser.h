#pragma once

#include "../Core/Defines.h"

#include <string>
#include <algorithm>
#include <map>
#include <sstream>
#include <cstdint>

#pragma warning( disable : 4251 )	//export std


AK_NAMESPACE_BEGIN

class AK_DLL Option
{
public:
	enum class Type : uint8_t
	{
		Undefined = 0,
		Integer,
		Unsigned,
		Float,
		String,
		Bool
	};

	AK_INLINE Option() : m_isMandatory(false), m_isSet(false) {}
	AK_INLINE virtual ~Option() {}

	AK_INLINE virtual bool NeedValue() const { return false; }
	AK_INLINE virtual bool HasDefault() const { return false; }

	AK_INLINE virtual Type GetType() const { return Type::Undefined; }

	AK_INLINE void SetOption(const std::string& _option) { m_option = _option; }
	AK_INLINE bool IsSet() const { return m_isSet; }
	AK_INLINE bool IsMandatory() const { return m_isMandatory; }
	AK_INLINE void Set() { m_isSet = true; }
	AK_INLINE void SetMandatory(bool _isMandatory) { m_isMandatory = _isMandatory; }
	AK_INLINE const std::string& GetOption() const { return m_option; }
	AK_INLINE bool HasValue() const { return !m_stringValue.empty(); }
	void SetStringValue(const std::string& _value);

protected:
	std::string m_option;
	std::string m_stringValue;
	bool m_isMandatory;
	bool m_isSet;
};

//////////////////////////////////////////////////////////////////////////

class OptionValueInteger : public Option
{
public:
	OptionValueInteger() : Option(), m_hasDefault(false), m_value(0), m_default(0) {}
	virtual ~OptionValueInteger() {}
	virtual bool NeedValue() const override { return true; }
	virtual bool HasDefault() const override { return m_hasDefault; }
	virtual Type GetType() const override { return Type::Integer; }
	int32_t GetValue() const { return HasValue() ? m_value : m_default; }
	void SetDefault(int32_t _value);
	void Parse();

private:
	int32_t m_value;
	int32_t m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

class OptionValueUnsigned : public Option
{
public:
	OptionValueUnsigned() : Option(), m_hasDefault(false), m_value(0), m_default(0) {}
	virtual ~OptionValueUnsigned() {}
	virtual bool NeedValue() const override { return true; }
	virtual bool HasDefault() const override { return m_hasDefault; }
	virtual Type GetType() const override { return Type::Unsigned; }
	uint32_t GetValue() const { return HasValue() ? m_value : m_default; }
	void SetDefault(uint32_t _value);
	void Parse();

private:
	uint32_t m_value;
	uint32_t m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

class OptionValueFloat : public Option
{
public:
	OptionValueFloat() : Option(), m_hasDefault(false), m_value(0.0f), m_default(0.0f) {}
	virtual ~OptionValueFloat() {}
	virtual bool NeedValue() const override { return true; }
	virtual bool HasDefault() const override { return m_hasDefault; }
	virtual Type GetType() const override { return Type::Float; }
	float GetValue() const { return HasValue() ? m_value : m_default; }
	void SetDefault(float _value);
	void Parse();

private:
	float m_value;
	float m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

class OptionValueBool : public Option
{
public:
	OptionValueBool() : Option(), m_hasDefault(false), m_value(0.0f), m_default(0.0f) {}
	virtual ~OptionValueBool() {}
	virtual bool NeedValue() const override { return true; }
	virtual bool HasDefault() const override { return m_hasDefault; }
	virtual Type GetType() const override { return Type::Bool; }
	bool GetValue() const { return HasValue() ? m_value : m_default; }
	void SetDefault(bool _value);
	void Parse();

private:
	bool m_value;
	bool m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

class OptionValueString : public Option
{
public:
	OptionValueString() : Option(), m_hasDefault(false) {}
	virtual ~OptionValueString() {}

	virtual bool NeedValue() const override { return true; }
	virtual bool HasDefault() const override { return m_hasDefault; }
	virtual Type GetType() const override { return Type::String; }
	const std::string& GetValue() const { return HasValue() ? m_value : m_default; }
	void SetDefault(std::string _value);
	void Parse();

private:
	std::string m_value;
	std::string m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

class AK_DLL CommandLineParser final
{
public:
	CommandLineParser();
	~CommandLineParser();

	void Add(const std::string& _option, bool _mandatory = true);
	void AddWithValueString(const std::string& _option, bool _mandatory = true);
	void AddWithValueAndDefaultString(const std::string& _option, bool _mandatory = true, const char* _default = "");
	void AddWithValueInteger(const std::string& _option, bool _mandatory = true);
	void AddWithValueAndDefaultInteger(const std::string& _option, bool _mandatory = true, int32_t _default = 0);
	void AddWithValueUnsigned(const std::string& _option, bool _mandatory = true);
	void AddWithValueAndDefaultUnsigned(const std::string& _option, bool _mandatory = true, uint32_t _default = 0u);
	void AddWithValueFloat(const std::string& _option, bool _mandatory = true);
	void AddWithValueAndDefaultFloat(const std::string& _option, bool _mandatory = true, float _default = 0.0f);
	void AddWithValueBool(const std::string& _option, bool _mandatory = true);
	void AddWithValueAndDefaultBool(const std::string& _option, bool _mandatory = true, bool _default = false);

	bool Parse(int32_t argc, const char * const argv[]);

	bool HasOption(const std::string& _option);

	bool HasValue(const std::string& _option);
	bool IsSet(const std::string& _option);

	// this one has to check the type and cast
	const Option* GetValue(const std::string& _option) const;
	bool GetValue(const std::string& _option, int32_t& _output) const;
	bool GetValue(const std::string& _option, uint32_t& _output) const;
	bool GetValue(const std::string& _option, float& _output) const;
	bool GetValue(const std::string& _option, std::string& _output) const;
	bool GetValue(const std::string& _option, bool& _output) const;

private:
	std::map<std::string, Option*> m_options;
};


AK_NAMESPACE_END