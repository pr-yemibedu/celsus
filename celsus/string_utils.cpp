#include "stdafx.h"
#include "celsus.hpp"
#include "string_utils.hpp"

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

string2 string2::empty_string;


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
	: _data(nullptr)
	, _len(0)
{
	assign(data);
}

string2::string2(const std::string& str)
	: _data(nullptr)
	, _len(0)
{
	assign(str.c_str());
}

string2::string2(const string2& str)
	: _data(nullptr)
	, _len(0)
{
	assign(str._data);
}

string2::string2(const char *data, int len)
	: _data(nullptr)
	, _len(0)
{
	assign(data, len);
}

string2& string2::operator=(const string2& str)
{
	if (&str != this)
		assign(str._data);
	return *this;
}

string2::operator const char *() const
{
	return _data;
}

void string2::assign(const char *data)
{
	if (!data) {
		delete exch_null(_data);
		_len = 0;
	} else {
		_len = strlen(data);
		delete [] exch(_data, new char[_len+1]);
		memcpy(_data, data, _len);
		_data[_len] = 0;
	}
}

void string2::assign(const char *data, const int len)
{
	delete exch_null(_data);
	if (!data || len == 0) {
		_len = 0;
	} else {
		_data = new char[len+1];
		memcpy(_data, data, len);
		_data[len] = 0;
		_len = len;
	}
}


void string2::append(const char *str, const int len)
{
  char *new_ptr = new char[_len + len + 1];
  memcpy(new_ptr, _data, _len);
  memcpy(new_ptr + _len, str, len);
  new_ptr[_len+len] = 0;
  delete [] exch(_data, new_ptr);
  _len += len;
}

void string2::append(const string2& str)
{
  append(str._data, str._len);
}

void string2::clear()
{
	SAFE_ADELETE(_data);
	_len = 0;
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

bool string2::operator!=(const string2& str) const
{
	return !operator==(str);
}

bool string2::operator!=(const char *str) const
{
	return !operator==(str);
}

size_t string2::calc_hash() const
{
  // djb2 (bernstein hash)
  size_t hash = 5381;
  char c;
  const char *str = _data;
  while (c = *str++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

string2 operator+(const string2& a, const string2& b)
{
  string2 res;
  res.append(a);
  res.append(b);
  return res;

}

string2& string2::operator+=(const string2& str)
{
	append(str);
	return *this;
}

string2& string2::operator+=(const char *str)
{
	append(str);
	return *this;
}

string2& string2::operator+=(const char ch)
{
	char buf[2] = { ch, 0 };
	append(buf);
	return *this;
}

string2 string2::substr(const int start, const int len) const
{
	if (_data == NULL || start + len >= _len)
		return string2();

	return string2(&_data[start], len);
}


bool operator<(const string2& lhs, const string2& rhs)
{
  return memcmp((const char *)lhs, (const char *)rhs, std::min<int>(lhs.size(), rhs.size())) < 0;
}

bool operator<(const string2& lhs, const char *rhs)
{
  return memcmp((const char *)lhs, rhs, std::min<int>(lhs.size(), strlen(rhs))) < 0;
}

bool operator<(const char *lhs, const string2& rhs)
{
  return memcmp(lhs, (const char *)rhs, std::min<int>(strlen(lhs), rhs.size())) < 0;
}
