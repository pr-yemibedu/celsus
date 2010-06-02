#ifndef _STRING_UTILS_HPP_
#define _STRING_UTILS_HPP_

bool ends_with(const char* str_to_test, const char* ending);

// So, I finally bit the bullet and decided to write my own string class.
// This isn't STL-compliant, and right now I don't have any need
// for something that's UTF-8, so I'll just hack at this, and see how it works
class string2
{
public:
	string2();
	~string2();

	// assignment
	string2(const char *data);
	string2(const std::string& str);
	string2(const string2& str);
	string2& operator=(const string2& str);
	operator const char *() const;

	// comparison
	bool operator==(const string2& str) const;
	bool operator==(const char *str) const;
	bool operator!=(const string2& str) const;
	bool operator!=(const char *str) const;

  friend string2 operator+(const string2& a, const string2& b);
  friend string2 operator+=(string2& a, const string2& b);
  friend string2 operator+=(string2& a, const char *b);

  bool operator<(const string2& str) const { return hash() < str.hash(); }

	int size() const { return _len; }
	bool empty() const { return _len == 0; }
	size_t hash() const;
	//operator size_t() const { return hash(); }

	static string2 fmt(const char *format, ...);
private:
	void assign(const char *data);
  void append(const char *str, const int len);
  void append(const string2& str);
  size_t calc_hash() const;
	char *_data;
	int _len;
  mutable size_t _hash;
  mutable bool _dirty;
};

template<>
struct std::less<string2>
{
	bool operator()(const string2& a, const string2& b) const
	{
		return a.hash() < b.hash();
	}
};

#endif
