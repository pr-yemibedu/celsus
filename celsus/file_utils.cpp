#include "stdafx.h"
#include "file_utils.hpp"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace
{
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

bool get_file_size(const char *filename, DWORD *low_size, DWORD *high_size)
{
  const HANDLE h = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (h == INVALID_HANDLE_VALUE)
    return false;

  *low_size = GetFileSize(h, high_size);

  CloseHandle(h);
  return true;
}

bool get_file_time(const char *filename, FILETIME *creation, FILETIME *access, FILETIME *write)
{
  const HANDLE h = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (h == INVALID_HANDLE_VALUE)
    return false;

  bool res = !!GetFileTime(h, creation, access, write);

  CloseHandle(h);
  return res;
}

bool file_exists(const char *filename)
{
  if (_access(filename, 0) != 0)
    return false;

  struct _stat status;
  if (_stat(filename, &status) != 0)
    return false;

  return !!(status.st_mode & _S_IFREG);
}
