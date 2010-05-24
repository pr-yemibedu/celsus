#include "stdafx.h"
#include "celsus.hpp"

uint8_t* load_file_inner(const char* filename, const bool zero_terminate, uint32_t* len)
{
	const HANDLE file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_handle == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	const uint32_t file_size = GetFileSize(file_handle, NULL);
	*len = file_size + (zero_terminate ? 1 : 0);
	uint8_t* buf = new BYTE[*len];
	DWORD bytes_read = 0;
	if (!ReadFile(file_handle, buf, file_size, &bytes_read, NULL) || bytes_read != file_size) {
		return NULL;
	}

	CloseHandle(file_handle);
	if (zero_terminate) {
		buf[file_size] = 0;
	}
	return buf;
}

uint8_t* load_file(const char* filename, uint32_t* len)
{
  return load_file_inner(filename, false, len);
}

uint8_t* load_file_with_zero_terminate(const char* filename, uint32_t* len)
{
  return load_file_inner(filename, true, len);
}

bool write_file(const uint8_t* buf, const uint32_t len, const char* filename)
{
#pragma warning(suppress: 4996)
  FILE* file = fopen(filename, "wb");
  if (file == NULL) {
    return false;
  }

  const uint32_t bytes_written = fwrite(buf, 1, len, file);
  if (bytes_written != len) {
    return false;
  }

  if (fclose(file) != 0) {
    return false;
  }
  return true;
}


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
