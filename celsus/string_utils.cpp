#include "stdafx.h"
#include "celsus.hpp"
#include "string_utils.hpp"

namespace
{

}

bool ends_with(const char* str_to_test, const char* ending)
{
	if (str_to_test == NULL || ending == NULL) {
		return false;
	}

	const int ending_len = strlen(ending);
	const int str_len = strlen(str_to_test);

	if (str_len	 < ending_len) {
		return false;
	}

	for (int i = 0; i < ending_len; ++i) {
		if (str_to_test[str_len - i - 1] != ending[ending_len - i - 1]) {
			return false;
		}
	}
	return true;
}


string2::string2()
	: _data(nullptr)
	, _len(0)
{
}

string2::~string2()
{
	SAFE_ADELETE(_data);
	_len = 0;
}

string2::string2(const char *data)
{
	assign(data);
}

string2::string2(const std::string& str)
{
	assign(str.c_str());
}

string2::string2(const string2& str)
{
	assign(str._data);
}

string2& string2::operator=(const string2& str)
{
	if (&str != this)
		assign(str._data);
	return *this;
}

string2::operator const char *()
{
	return _data;
}

void string2::assign(const char *data)
{
	_len = strlen(data);
	_data = new char[_len+1];
	memcpy(_data, data, _len);
	_data[_len] = 0;
}

string2 string2::fmt(const char *format, ...)
{
	string2 res;
	va_list arg;
	va_start(arg, format);
	res._len = _vscprintf(format, arg);
	res._data = new char[res._len + 1];
	vsprintf_s(res._data, res._len + 1, format, arg);
	va_end(arg);
	return res;
}

bool string2::operator==(const string2& str) const
{
	if (_len != str._len)
		return false;

	for (int i = 0; i < _len; ++i) {
		if (_data[i] != str._data[i])
			return false;
	}
	return true;
}

bool string2::operator==(const char *str) const
{
	const int len = strlen(str);
	if (_len != len)
		return false;

	for (int i = 0; i < _len; ++i) {
		if (_data[i] != str[i])
			return false;
	}

	return true;
}

// djb2 (bernstein hash)
long long string2::hash() const
{
	long long hash = 5381;
	char c;
	const char *str = _data;
	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}
