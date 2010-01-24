#include "stdafx.h"
#include "MemoryMappedFile.hpp"

MemoryMappedFile::MemoryMappedFile() 
: file_handle_(INVALID_HANDLE_VALUE) 
, file_mapping_(INVALID_HANDLE_VALUE)
, view_(NULL)
{
}

MemoryMappedFile::~MemoryMappedFile() 
{
  if (view_ != NULL) {
    UnmapViewOfFile(view_);
  }
  if (file_mapping_ != INVALID_HANDLE_VALUE) {
    CloseHandle(file_mapping_);
  }
  if (file_handle_ != INVALID_HANDLE_VALUE ) {
    CloseHandle(file_handle_);
  }
}

bool MemoryMappedFile::open(const char* filename, uint8_t*& data, uint32_t& data_len) 
{
  file_handle_ = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file_handle_ == INVALID_HANDLE_VALUE) {
    return false;
  }
  const DWORD file_size = GetFileSize(file_handle_, NULL);

  file_mapping_ = CreateFileMapping(file_handle_, NULL, PAGE_READONLY, 0, 0, 0);
  if (file_mapping_ == INVALID_HANDLE_VALUE) {
    return false;
  }
  view_ = MapViewOfFile(file_mapping_, FILE_MAP_READ, 0, 0, 0);
  data = reinterpret_cast<uint8_t*>(view_);
  data_len = file_size;
  return true;
}
