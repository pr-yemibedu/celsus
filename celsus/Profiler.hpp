#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <deque>
#include <list>
#include <stack>
#include <string>
#include <cassert>
#include <windows.h>
#include "celsus.hpp"

// Profiler singleton. Use the SCOPED_PROFILE macro to mark enter/leaving scope
class Profiler
{
public:
  struct Scope
  {
    Scope(const char* name, const LARGE_INTEGER& enter) 
      : name_(name)
      , enter_(enter)
    {
    }

    ~Scope()
    {
      container_delete(children_);
    }
    std::string name_;
    LARGE_INTEGER enter_;
    LARGE_INTEGER leave_;
    typedef std::list<Scope*> Children;
    Children children_;
  };

  static Profiler& instance();
  void enter_scope(const char* name);
  void leave_scope();
  void print();
  static void close();

private:
  void print_inner(const Scope* cur, const std::string& indent);
  Profiler();
  ~Profiler();

  static Profiler* instance_;

  LARGE_INTEGER frequency_;
  typedef std::list<Scope*> TopLevel;
  typedef std::deque<Scope*> ParentStack;
  TopLevel top_level_;
  ParentStack parent_stack_;
};

struct ScopedScope
{
  ScopedScope(const char* name)
  {
    Profiler::instance().enter_scope(name);
  }

  ~ScopedScope()
  {
    Profiler::instance().leave_scope();
  }
};

#define SCOPED_PROFILE(name) ScopedScope GEN_NAME(log, __LINE__)(name);
#define SCOPED_FUNC_PROFILE() ScopedScope GEN_NAME(log, __LINE__)(__FUNCTION__);

#endif
