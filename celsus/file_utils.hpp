#pragma once
#include <vector>
#include "string_utils.hpp"
#include "celsus.hpp"

bool file_exists(const char *filename);
bool get_file_size(const char *filename, DWORD *low_size, DWORD *high_size);
bool get_file_time(const char *filename, FILETIME *creation, FILETIME *access, FILETIME *write);

bool write_file(const uint8_t* buf, const uint32_t len, const char* filename);

bool load_file(const char* filename, AsArray<byte> *data);

uint8_t* load_file(const char* filename, uint32_t* len);
uint8_t* load_file_with_zero_terminate(const char* filename, uint32_t* len);

class FileWriter
{
public:
	FileWriter();
	~FileWriter();
	bool open(const char *filename);
	bool write_string(const string2& str);
	template<typename T> bool write(const T& t) { return write_raw((const void *)&t, sizeof(T)); }
	template<typename T> bool write_vector(const std::vector<T>& v) 
	{ 
		// note, we save the vector as raw bytes, instead of as T
		return write(v.size() * sizeof(T)) && write_raw((const void *)&v[0], v.size() * sizeof(T)); 
	}
	bool write_raw(const void *buf, const DWORD len);
private:
	HANDLE _file;

};

class FileReader
{
public:
	FileReader();
	bool load(const char *filename);
	uint8_t *data() const { return _data; }
	int32_t len() const { return _len; }

	void add_ref() const;
	void release() const;
private:
	~FileReader();
	mutable LONG _ref_count;
	uint8_t *_data;
	int32_t _len;

};

// Silly name, but the idea is that a DataReader takes a file, and you use it
// to iterate over the data in the file
class DataReader
{
public:
	DataReader(FileReader *file);
	DataReader(uint8_t *data, int32_t len);
	~DataReader();
	template<typename T>
	bool read(T *out)
	{
		if (_ofs + (int32_t)sizeof(T) > _len)
			return false;

		*out = *(T*)(_data + _ofs);
		_ofs += sizeof(T);
		return true;
	}
	bool read_string(string2 *out);
	template<typename T>
	bool read_vector(std::vector<T> *out)
	{
		out->clear();
		int num_elems;
		// num elems is really num bytes
		if (!read(&num_elems))
			return false;
		const int data_size = num_elems;
		num_elems /= sizeof(T);
		if (_ofs + data_size > _len)
			return false;
		out->resize(num_elems);
		memcpy(&out->operator[](0), _data + _ofs, data_size);
		_ofs += data_size;
		return true;
	};
private:
	FileReader *_file;
	uint8_t *_data;
	int32_t _len;
	int32_t _ofs;

};


bool save_bmp32(const char *filename, uint8_t *ptr, int width, int height);
