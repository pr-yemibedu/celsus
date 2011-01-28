#ifndef CELSUS_HPP
#define CELSUS_HPP

#include <functional>
#include <windows.h>
#include "string_utils.hpp"

#define SAFE_RELEASE(x) if( (x) != 0 ) { (x)->Release(); (x) = 0; }
#define SAFE_FREE(x) if( (x) != 0 ) { free((void*)(x)); (x) = 0; }
#define SAFE_DELETE(x) if( (x) != 0 ) { delete (x); (x) = 0; }
#define SAFE_ADELETE(x) if( (x) != 0 ) { delete [] (x); (x) = 0; }

// Macro for creating "local" names
#define GEN_NAME2(prefix, line) prefix##line
#define GEN_NAME(prefix, line) GEN_NAME2(prefix, line)
#define MAKE_SCOPED(type) type GEN_NAME(ANON, __LINE__)
// #define SCOPED_PROFILE(s) LogMgr::Scope GEN_NAME(log, __LINE__)(s);

// Calls fn when it goes out of scope. With lambda functions in C++0x this is actually
// pretty useful
struct ScopedObj
{
  typedef std::function<void()> Fn;
  ScopedObj(const Fn& fn) : fn(fn) {}
  ~ScopedObj() { fn(); }
  Fn fn;
};

struct ScopedCs
{
	ScopedCs(CRITICAL_SECTION* cs) : _cs(cs) { EnterCriticalSection(_cs); }
	~ScopedCs() { LeaveCriticalSection(_cs); }
	CRITICAL_SECTION* _cs;
};

#define SCOPED_CS(x) MAKE_SCOPED(ScopedCs)((x));
#define SCOPED_OBJ(x) MAKE_SCOPED(ScopedObj)((x));

template<typename T>
T exch_null(T& t)
{
  T tmp = t; t = nullptr; return tmp;
}

template<typename T>
T exch(T& a, T& b)
{
  T tmp = a; a = b; b = tmp; return tmp;
}

template<typename T>
T exch_const(T& a, const T& b)
{
  T tmp = a; a = b; return tmp;
}

template< class Container >
void safe_erase(Container& c, const typename Container::value_type& v)
{
	auto it = std::find(c.begin(), c.end(), v);
	if (it != c.end())
		c.erase(it);
}

template<class T>
void container_delete(T& container)
{
  for (T::iterator it = container.begin(), e = container.end(); it != e; ++it) {
    delete (*it);
  }
  container.clear();
}

template<class T>
void map_delete(T& container)
{
  for (T::iterator it = container.begin(), e = container.end(); it != e; ++it) {
    delete it->second;
  }
  container.clear();
}

#define ELEMS_IN_ARRAY(x) sizeof(x) / sizeof((x)[0])

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

string2 get_env_variable(const char *var);

float randf(const float min_value, const float max_value);

// wrapper around a <data,size> tuple
template<typename T>
class AsArray
{
public:
  AsArray(T* data, int n) : _data(data), _n(n) {}
  AsArray(std::vector<T>& v) : _data(&v[0]), _n((int)v.size()) {}
  int size() const { return _n; }
  T *data() { return _data; }
private:
  T *_data;
  int _n;
};

template<typename T> AsArray<T> as_array(T *data, int len) { return AsArray<T>(data, len); }


#endif // #ifndef CELSUS_HPP