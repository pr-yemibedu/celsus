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


string2 get_env_variable(const char *var)
{
	string2 res;
	char *env = NULL;
	DWORD len = GetEnvironmentVariable(var, NULL, 0);
	env = new char[len + 1];
	GetEnvironmentVariable(var, env, len);
	res.assign(env, len);
	SAFE_ADELETE(env);
	return res;
}

