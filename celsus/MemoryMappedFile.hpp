#ifndef MEMORY_MAPPED_FILE_HPP
#define MEMORY_MAPPED_FILE_HPP

#include <stdint.h>

class MemoryMappedFile
{
public:
  MemoryMappedFile();
  ~MemoryMappedFile();

  bool open(const char* filename, uint8_t*& data, uint32_t& data_len);
private:
  HANDLE file_handle_;
  HANDLE file_mapping_;
  void* view_;
};

#endif 