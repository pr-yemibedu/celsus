#ifndef CELSUS_HPP
#define CELSUS_HPP

#define SAFE_RELEASE(x) if( (x) != 0 ) { (x)->Release(); (x) = 0; }
#define SAFE_FREE(x) if( (x) != 0 ) { free((void*)(x)); (x) = 0; }
#define SAFE_DELETE(x) if( (x) != 0 ) { delete (x); (x) = 0; }
#define SAFE_ADELETE(x) if( (x) != 0 ) { delete [] (x); (x) = 0; }

// Macro for creating "local" names
#define GEN_NAME2(prefix, line) prefix##line
#define GEN_NAME(prefix, line) GEN_NAME2(prefix, line)
#define MAKE_SCOPED(type) type GEN_NAME(ANON, __LINE__)
// #define SCOPED_PROFILE(s) LogMgr::Scope GEN_NAME(log, __LINE__)(s);

struct ScopedCs
{
	ScopedCs(CRITICAL_SECTION* cs) : _cs(cs) { EnterCriticalSection(_cs); }
	~ScopedCs() { LeaveCriticalSection(_cs); }
	CRITICAL_SECTION* _cs;
};

#define SCOPED_CS(x) MAKE_SCOPED(ScopedCs)(x);

template<typename T>
T xchg_null(T& t)
{
  T tmp = t; t = NULL; return tmp;
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

#endif // #ifndef CELSUS_HPP