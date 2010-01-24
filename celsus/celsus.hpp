#ifndef CELSUS_HPP
#define CELSUS_HPP

#include <stdint.h>
#include <vector>
#include "Logger.hpp"
#include "ErrorHandling.hpp"
#include <boost/call_traits.hpp>

#define SAFE_RELEASE(x) if( (x) != 0 ) { (x)->Release(); (x) = 0; }
#define SAFE_FREE(x) if( (x) != 0 ) { free((void*)(x)); (x) = 0; }
#define SAFE_DELETE(x) if( (x) != 0 ) { delete (x); (x) = 0; }
#define SAFE_ADELETE(x) if( (x) != 0 ) { delete [] (x); (x) = 0; }

// Macro for creating "local" names
#define GEN_NAME2(prefix, line) prefix##line
#define GEN_NAME(prefix, line) GEN_NAME2(prefix, line)
#define MAKE_SCOPED(type) type GEN_NAME(ANON, __LINE__)
// #define SCOPED_PROFILE(s) LogMgr::Scope GEN_NAME(log, __LINE__)(s);


template<class T>
void container_delete(T& container)
{
  for (T::iterator it = container.begin(); it != container.end(); ++it) {
    delete (*it);
  }
  container.clear();
}

// "swap trick" to clear a stl vector
template< class T >
void clear_vector(T& t)
{
  t.swap(T());
}


// Helper function for looking up a value in an associated container. Throws an exception if the value isn't found.

// hack to get the key name is it's a string
template< typename T > struct ToStringHack;
template<typename T> struct ToStringHack {
  std::string operator ()(T) const { return "unknown"; }
};
template<> struct ToStringHack<const std::string&> {
  std::string operator ()(const std::string& str) const { return str; }
};

template< class T >
struct param_type {
  typedef typename boost::call_traits<T>::param_type type;
};

template<class Container>
bool exists_in_container(const Container& cont, typename param_type<typename Container::key_type>::type value) {
  return cont.find(value) != cont.end();
}

template<class Container>
typename Container::iterator SafeFind(Container& container, typename param_type<typename Container::key_type>::type key ) {
  const typename Container::iterator it = container.find(key);
  SUPER_ASSERT(it != container.end())(key);
  return it;
}

template<class Container>
typename Container::mapped_type safe_find_value(Container& container, typename param_type<typename Container::key_type>::type key ) {
  return SafeFind(container, key)->second;
}

template<class Container>
typename Container::const_iterator SafeFind(const Container& container, typename param_type<typename Container::key_type>::type key ) {
  const typename Container::const_iterator it = container.find(key);
  SUPER_ASSERT(it != container.end())(key);
  return it;
}

#define ELEMS_IN_ARRAY(x) sizeof(x) / sizeof((x)[0])


bool load_file(uint8_t*& buf, uint32_t& len, const char* filename, const bool zero_terminate = false);
bool write_file(const uint8_t* buf, const uint32_t len, const char* filename);


float randf(const float min_value, const float max_value);


#endif // #ifndef CELSUS_HPP