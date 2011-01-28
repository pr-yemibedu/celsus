#include "stdafx.h"
#include "MemoryMappedFile.hpp"

MemoryMappedFile::MemoryMappedFile() 
  : _file_handle(INVALID_HANDLE_VALUE) 
  , _file_mapping(INVALID_HANDLE_VALUE)
  , _view(NULL)
{
}

MemoryMappedFile::~MemoryMappedFile() 
{
  if (_view != NULL)
    UnmapViewOfFile(_view);

  if (_file_mapping != INVALID_HANDLE_VALUE)
    CloseHandle(_file_mapping);

  if (_file_handle != INVALID_HANDLE_VALUE )
    CloseHandle(_file_handle);

}

bool MemoryMappedFile::open(const char* filename, void** data, uint64_t* data_len, size_t lock_size)
{
  _file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (_file_handle == INVALID_HANDLE_VALUE)
    return false;

  DWORD hi, lo = GetFileSize(_file_handle, &hi);

  _file_mapping = CreateFileMapping(_file_handle, NULL, PAGE_READONLY, 0, 0, 0);
  if (_file_mapping == INVALID_HANDLE_VALUE)
    return false;

  *data = _view = MapViewOfFile(_file_mapping, FILE_MAP_READ, 0, 0, lock_size);
	if (!*data) {
		DWORD err = GetLastError();
		return false;
	}
  *data_len = (uint64_t)hi << 32 | lo;
  return true;
}

bool MemoryMappedFile::lock(uint64_t ofs, size_t len)
{
	if (_view)
		UnmapViewOfFile(_view);

	_view = MapViewOfFile(_file_mapping, FILE_MAP_READ, (DWORD)(ofs >> 32), (DWORD)(ofs & 0xffffffff), len);
	return true;
}
