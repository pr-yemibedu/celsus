#include "stdafx.h"
#include "path_utils.hpp"


std::string Path::make_canonical(const std::string& str)
{
  // convert back slashes to forward

  std::string res;
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


Path::Path(const std::string& str) 
: _str(make_canonical(str)) 
, _ext_ofs(-1)
{
  for (int i = 0, e = _str.size(); i < e; ++i) {
    if (_str[i] == '.') {
      _ext_ofs = i;
      break;
    }
  }
}

Path Path::replace_extension(const std::string& ext)
{
  if (_ext_ofs == -1) {
    return Path(_str + "." + ext);
  }

  return Path(std::string(_str.c_str(), _ext_ofs) + "." + ext);
}

const std::string& Path::str() const
{
  return _str;
}

std::string Path::get_path() const
{
  return "";
}

std::string Path::get_ext() const
{
  return "";
}

std::string Path::get_filename() const
{
  return "";
}

std::string Path::get_filename_without_ext() const
{
  return "";
}

std::string Path::get_full_path_name(const std::string& p)
{
	char buf[MAX_PATH];
	GetFullPathName(p.c_str(), MAX_PATH, buf, NULL);
	return buf;
}


