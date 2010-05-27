#include "stdafx.h"
#include "celsus.hpp"



float randf(const float min_value, const float max_value)
{
  const float span = max_value - min_value;
  const float r = rand() / (float)RAND_MAX;
  return min_value + r * span;
}

std::string to_string(char const * const format, ... ) 
{
  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);
  va_end(arg);

  return std::string(buf);
}
