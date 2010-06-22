#ifndef _PATH_UTILS_HPP_
#define _PATH_UTILS_HPP_
#include "string_utils.hpp"

class Path
{
public:
  Path(const string2& str);
  Path replace_extension(const string2& ext);
  const string2& str() const;
  string2 get_path() const;
  string2 get_ext() const;
  string2 get_filename() const;
  string2 get_filename_without_ext() const;

	static string2 make_canonical(const string2& str);
	static string2 get_full_path_name(const string2& p);
  static string2 replace_extension(const string2& path, const string2& ext);

private:
  string2 _str;
  int _file_ofs;		// points to the last '/'
  int _ext_ofs;			// points to the '.'
};

#endif