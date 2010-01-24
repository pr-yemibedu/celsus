#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include "Logger.hpp"
#include <sstream>

// Stolen from SUPER_ASSERT, http://www.ddj.com/cpp/184403745 
/**
 * Muncher is combined with the MUNCHER macros below, and is a 
 * trick for macro expansion.
 */ 
struct Muncher
{
  Muncher(const char* expr, bool throw_exception) 
#pragma warning( suppress: 4355)
    : expr_(expr), throw_exception_(throw_exception), MUNCHER_A(*this), MUNCHER_B(*this) {}
  ~Muncher() { 
    if (throw_exception_) {
      throw std::exception(expr_); 
    } else {
      __asm int 3;
    }
  }
  template<class T>
  Muncher& PrintCurrentVal(const T& t, const char* name) {
    std::ostringstream o_stream;
    o_stream << name << " = " << t;
    LOG_INFO_LN(o_stream.str().c_str());
    return *this;
  }

  Muncher& PrintContext(const char* file, const int line, const char* function) {
    LOG_INFO_LN("** %s failed", expr_);
    LOG_INFO_LN("%s(%d): %s", file, line, function);
    return *this;
  }
  bool throw_exception_;
  const char* expr_;
  Muncher& MUNCHER_A;
  Muncher& MUNCHER_B;
  void operator=(const Muncher&);
};

#define MUNCHER_A(x) MUNCHER_OP(x, B)
#define MUNCHER_B(x) MUNCHER_OP(x, A)
#define MUNCHER_OP(x, next) \
  MUNCHER_A.PrintCurrentVal((x), #x).MUNCHER_ ## next

inline Muncher MakeMuncher(const char* expr, const bool throw_exception) {
  return Muncher(expr, throw_exception);
}
/**
 * ENFORCE_HR throws an exception if the expression fails. Extra values can be passed like this:
 * ENFORCE_HR(expr)(value1)(value2) etc
 */ 
#define ENFORCE_HR(expr) \
  if ( SUCCEEDED(expr) ) ; \
    else MakeMuncher(#expr, true).PrintContext( __FILE__, __LINE__, __FUNCTION__).MUNCHER_A

/**
 * ENFORCE throws if the expression fails
 */ 
#define ENFORCE(expr) \
  if ( (expr) ) ; \
    else MakeMuncher(#expr, true).PrintContext( __FILE__, __LINE__, __FUNCTION__).MUNCHER_A

/**
 * Standard assert macro, but extra values can be given.
 */ 
#define SUPER_ASSERT(expr) \
  if ( (expr) ) ; \
    else MakeMuncher(#expr, false).PrintContext( __FILE__, __LINE__, __FUNCTION__).MUNCHER_A

#endif // #ifndef ERROR_HANDLING_HPP
