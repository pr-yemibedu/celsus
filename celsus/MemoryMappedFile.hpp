#ifndef MEMORY_MAPPED_FILE_HPP
#define MEMORY_MAPPED_FILE_HPP

#include <stdint.h>
#include <windows.h>

class MemoryMappedFile
{
public:
  MemoryMappedFile();
  ~MemoryMappedFile();

  bool open(const char* filename, void** data, uint64_t* data_len, size_t lock_size);
	bool lock(uint64_t ofs, size_t len);
private:
  HANDLE _file_handle;
  HANDLE _file_mapping;
  void* _view;
};

#endif 