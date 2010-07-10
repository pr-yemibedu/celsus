#include "stdafx.h"
#include "path_utils.hpp"


string2 Path::make_canonical(const string2& str)
{
  // convert back slashes to forward

  string2 res;
  for (int i = 0, e = str.size(); i < e; ++i) {
    const char ch = str[i];
    if (ch == '\\') {
      res += '/';
    } else {
      res += ch;
    }
  }
  return res;
}


Path::Path(const string2& str) 
: _str(make_canonical(str)) 
, _ext_ofs(-1)
{
  for (int i = _str.size() - 1; i >= 0; --i) {
		char ch = _str[i];
    if (ch == '.') {
			_ext_ofs = i;
		} else if (ch == '/') {
			// last slash
			_file_ofs = i;
			break;
		}
  }
}

Path Path::replace_extension(const string2& ext)
{
  if (_ext_ofs == -1) {
    return Path(_str + "." + ext);
  }

  return Path(string2(_str, _ext_ofs) + "." + ext);
}

const string2& Path::str() const
{
  return _str;
}

string2 Path::get_path() const
{
	string2 res;
	if (_file_ofs != -1)
		res = _str.substr(0, _file_ofs + 1);
	return res;
}

string2 Path::get_ext() const
{
	string2 res;
	if (_ext_ofs != -1)
		res.assign(&_str[_ext_ofs+1]);
	return res;
}

string2 Path::get_filename() const
{
	string2 res;
	if (_file_ofs != -1)
		res.assign(&_str[_file_ofs+1]);
	return res;
}

string2 Path::get_filename_without_ext() const
{
	string2 res;
	if (_file_ofs != -1) {
		// 0123456
		// /tjong.ext
		// ^     ^--- _ext_ofs
		// +--------- _file_ofs
		int end = _ext_ofs == -1 ? _str.size() : _ext_ofs;
		res = _str.substr(_file_ofs + 1, end - _file_ofs - 1);
	}
  return res;
}

string2 Path::get_full_path_name(const string2& p)
{
	char buf[MAX_PATH];
	GetFullPathName(p, MAX_PATH, buf, NULL);
	return buf;
}

string2 Path::replace_extension(const string2& path, const string2& ext)
{
	string2 res;
	if (!path.empty() && !ext.empty()) {
		const char *dot = path;
		while (*dot && *dot++ != '.')
			;

		if (*dot)
			res = string2(path, dot - path) + ext;
		else
			res = path + (dot[-1] == '.' ? "" : ".") + ext;
	}

  return res;
}

string2 Path::get_path(const string2& p)
{
	Path a(p);
	return a.get_path();
}
