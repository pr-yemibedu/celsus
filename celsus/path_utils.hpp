#ifndef _PATH_UTILS_HPP_
#define _PATH_UTILS_HPP_

class Path
{
public:
  Path(const std::string& str);
  Path replace_extension(const std::string& ext);
  const std::string& str() const;
  std::string get_path() const;
  std::string get_ext() const;
  std::string get_filename() const;
  std::string get_filename_without_ext() const;
	static std::string make_canonical(const std::string& str);
	static std::string get_full_path_name(const std::string& p);

  static std::string replace_extension(const std::string& path, const std::string& ext);

private:
  std::string _str;
  int _file_ofs;
  int _ext_ofs;
};

#endif