#pragma once

bool file_exists(const char *filename);
bool get_file_size(const char *filename, DWORD *low_size, DWORD *high_size);
bool get_file_time(const char *filename, FILETIME *creation, FILETIME *access, FILETIME *write);

bool write_file(const uint8_t* buf, const uint32_t len, const char* filename);

uint8_t* load_file(const char* filename, uint32_t* len);
uint8_t* load_file_with_zero_terminate(const char* filename, uint32_t* len);


class FileReader
{
public:
	FileReader();
	bool load(const char *filename);
	uint8_t *data() const { return _data; }
	int32_t len() const { return _len; }
private:
	uint8_t *_data;
	int32_t _len;

};

// Silly name, but the idea is that a DataReader takes a file, and you use it
// to iterate over the data in the file
class DataReader
{
public:
	DataReader(uint8_t *data, int32_t len);
	template<typename T>
	bool read(T *out)
	{
		if (_ofs + sizeof(T) > _len)
			return false;

		*out = *(T*)&(_data + _ofs);
		_ofs += sizeof(T);
		return true;
	}
private:
	uint8_t *_data;
	int32_t _len;
	int32_t _ofs;

};


bool save_bmp32(const char *filename, uint8_t *ptr, int width, int height);
