#include "CommandLineParser.h"


AK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void Option::SetStringValue(const std::string& _value)
{
	m_stringValue = _value;
	std::transform(m_stringValue.begin(), m_stringValue.end(), m_stringValue.begin(), ::tolower);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void OptionValueInteger::SetDefault(int32_t _value)
{
	m_default = _value;
	m_hasDefault = true;
}

void OptionValueInteger::Parse()
{
	m_value = m_stringValue.empty() ? 0 : stoi(m_stringValue);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

void OptionValueUnsigned::SetDefault(uint32_t _value)
{
	m_default = _value;
	m_hasDefault = true;
}

void OptionValueUnsigned::Parse()
{
	m_value = m_stringValue.empty() ? 0 : stoi(m_stringValue);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void OptionValueString::SetDefault(std::string _value)
{
	m_default = _value;
	m_hasDefault = true;
}

void OptionValueString::Parse()
{
	m_value = m_stringValue;
	std::transform(m_value.begin(), m_value.end(), m_value.begin(), ::tolower);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void OptionValueFloat::SetDefault(float _value)
{
	m_default = _value;
	m_hasDefault = true;
}

void OptionValueFloat::Parse()
{
	m_value = m_stringValue.empty() ? 0.0f : stof(m_stringValue);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void OptionValueBool::SetDefault(bool _value)
{
	m_default = _value;
	m_hasDefault = true;
}

void OptionValueBool::Parse()
{
	if (!m_stringValue.empty())
	{
		std::transform(m_stringValue.begin(), m_stringValue.end(), m_stringValue.begin(), ::tolower);
		std::istringstream is(m_stringValue);
		is >> std::boolalpha >> m_value;
	}
	else
	{
		m_value = false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

CommandLineParser::CommandLineParser() {}

CommandLineParser::~CommandLineParser()
{
	std::map<std::string, Option*>::iterator it;
	for (it = m_options.begin(); it != m_options.end(); ++it)
	{
		delete it->second;
	}
	m_options.clear();
}

void CommandLineParser::Add(const std::string& _option, bool _mandatory)
{
	Option* opt = new Option;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueString(const std::string& _option, bool _mandatory /*= true*/)
{
	OptionValueString* opt = new OptionValueString;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueAndDefaultString(const std::string& _option, bool _mandatory /*= true*/, const char* _default /*= ""*/)
{
	OptionValueString* opt = new OptionValueString;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);
	opt->SetDefault(_default);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueInteger(const std::string& _option, bool _mandatory /*= true*/)
{
	OptionValueInteger* opt = new OptionValueInteger;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueAndDefaultInteger(const std::string& _option, bool _mandatory /*= true*/, int32_t _default /*= 0*/)
{
	OptionValueInteger* opt = new OptionValueInteger;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);
	opt->SetDefault(_default);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueUnsigned(const std::string& _option, bool _mandatory/* = true*/)
{
	OptionValueUnsigned* opt = new OptionValueUnsigned;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueAndDefaultUnsigned(const std::string& _option, bool _mandatory /*= true*/, uint32_t _default /*= 0*/)
{
	OptionValueUnsigned* opt = new OptionValueUnsigned;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);
	opt->SetDefault(_default);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueFloat(const std::string& _option,  bool _mandatory /*= true*/)
{
	OptionValueFloat* opt = new OptionValueFloat;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueAndDefaultFloat(const std::string& _option, bool _mandatory /*= true*/, float _default /*= 0.0f*/)
{
	OptionValueFloat* opt = new OptionValueFloat;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);
	opt->SetDefault(_default);

	m_options[_option] = opt;
}


void CommandLineParser::AddWithValueBool(const std::string& _option,  bool _mandatory /*= true*/)
{
	OptionValueBool* opt = new OptionValueBool;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);

	m_options[_option] = opt;
}

void CommandLineParser::AddWithValueAndDefaultBool(const std::string& _option, bool _mandatory /*= true*/, bool _default /*= 0.0f*/)
{
	OptionValueBool* opt = new OptionValueBool;
	opt->SetOption(_option);
	opt->SetMandatory(_mandatory);
	opt->SetDefault(_default);

	m_options[_option] = opt;
}


bool CommandLineParser::Parse(int32_t argc, const char * const argv[])
{
	// Parse the command line and set the value to the option
	// can fail if any errors occurred
	for (int32_t i = 1; i < argc; ++i)
	{
		if (strncmp(argv[i], "-", 1) == 0)
		{
			std::string name(argv[i]);
			if (m_options.count(name) == 0)
			{
				if (m_options[name]->IsMandatory())
				{
					return false;
				}
				else
				{
					continue;
				}
			}
			else
			{
				m_options[name]->Set();
			}

			if (i + 1 >= argc)
			{
				if (m_options[name]->NeedValue())
				{
					return false;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (m_options[name]->NeedValue())
				{
					++i;
					if (strncmp(argv[i], "-", 1) != 0)
					{
						m_options[name]->SetStringValue(argv[i]);
					}
					else if (m_options[name]->HasDefault())
					{
						continue;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool CommandLineParser::HasOption(const std::string& _option)
{
	return m_options.count(_option) > 0;
}

bool CommandLineParser::HasValue(const std::string& _option)
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		return search->second->HasValue();
	}
	else
	{
		return false;
	}
}

bool CommandLineParser::IsSet(const std::string& _option)
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		return search->second->IsSet();
	}
	else
	{
		return false;
	}
}

const Option* CommandLineParser::GetValue(const std::string& _option) const
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		Option* opt = search->second;
		if (opt->HasValue() || opt->HasDefault())
		{
			return opt;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

bool CommandLineParser::GetValue(const std::string& _option, int32_t& _output) const
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		Option* opt = search->second;
		if (opt->HasValue() || opt->HasDefault())
		{
			if (opt->GetType() == Option::Type::Integer)
			{
				OptionValueInteger* optInt = dynamic_cast<OptionValueInteger*>(opt);
				optInt->Parse();
				_output = optInt->GetValue();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


bool CommandLineParser::GetValue(const std::string& _option, uint32_t& _output) const
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		Option* opt = search->second;
		if (opt->HasValue() || opt->HasDefault())
		{
			if (opt->GetType() == Option::Type::Unsigned)
			{
				OptionValueUnsigned* optUns = dynamic_cast<OptionValueUnsigned*>(opt);
				optUns->Parse();
				_output = optUns->GetValue();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CommandLineParser::GetValue(const std::string& _option, float& _output) const
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		Option* opt = search->second;
		if (opt->HasValue() || opt->HasDefault())
		{
			if (opt->GetType() == Option::Type::Float)
			{
				OptionValueFloat* optFlt = dynamic_cast<OptionValueFloat*>(opt);
				optFlt->Parse();
				_output = optFlt->GetValue();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CommandLineParser::GetValue(const std::string& _option, std::string& _output) const
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		Option* opt = search->second;
		if (opt->HasValue() || opt->HasDefault())
		{
			if (opt->GetType() == Option::Type::Float)
			{
				OptionValueString* optStr = dynamic_cast<OptionValueString*>(opt);
				optStr->Parse();
				_output = optStr->GetValue();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


bool CommandLineParser::GetValue(const std::string& _option, bool& _output) const
{
	auto search = m_options.find(_option);
	if (search != m_options.end())
	{
		Option* opt = search->second;
		if (opt->HasValue() || opt->HasDefault())
		{
			if (opt->GetType() == Option::Type::Bool)
			{
				OptionValueBool* optBool = dynamic_cast<OptionValueBool*>(opt);
				optBool->Parse();
				_output = optBool->GetValue();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


AK_NAMESPACE_END