#pragma once

bool file_exists(const char *filename);
bool get_file_size(const char *filename, DWORD *low_size, DWORD *high_size);
bool get_file_time(const char *filename, FILETIME *creation, FILETIME *access, FILETIME *write);

bool write_file(const uint8_t* buf, const uint32_t len, const char* filename);

uint8_t* load_file(const char* filename, uint32_t* len);
uint8_t* load_file_with_zero_terminate(const char* filename, uint32_t* len);
