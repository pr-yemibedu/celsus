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

FileReader::FileReader()
	: _data(nullptr)
	, _len(0)
{
}

bool FileReader::load(const char *filename)
{
	uint32_t len = 0;
	_data = load_file(filename, &len);
	_len = len;
	return _data != NULL;
}


DataReader::DataReader(uint8_t *data, int32_t len) 
	: _data(data)
	, _len(len)
	, _ofs(0) 
{
}


bool save_bmp32(const char *filename, uint8_t *ptr, int width, int height)
{
	FILE *f = fopen(filename, "wb");
	if (!f)
		return false;

#define BITMAP_SIGNATURE 'MB'

#pragma pack(push, 1)

	typedef struct {
		unsigned short int Signature;
		unsigned int Size;
		unsigned int Reserved;
		unsigned int BitsOffset;
	} BITMAP_FILEHEADER;

#define BITMAP_FILEHEADER_SIZE 14

	typedef struct {
		unsigned int HeaderSize;
		int Width;
		int Height;
		unsigned short int Planes;
		unsigned short int BitCount;
		unsigned int Compression;
		unsigned int SizeImage;
		int PelsPerMeterX;
		int PelsPerMeterY;
		unsigned int ClrUsed;
		unsigned int ClrImportant;
		unsigned int RedMask;
		unsigned int GreenMask;
		unsigned int BlueMask;
		unsigned int AlphaMask;
		unsigned int CsType;
		unsigned int Endpoints[9]; // see http://msdn2.microsoft.com/en-us/library/ms536569.aspx
		unsigned int GammaRed;
		unsigned int GammaGreen;
		unsigned int GammaBlue;
	} BITMAP_HEADER;

#pragma pack(pop)

	BITMAP_FILEHEADER fh;
	BITMAP_HEADER h;

	ZeroMemory(&fh, sizeof(fh));
	ZeroMemory(&h, sizeof(h));

	fh.Signature = BITMAP_SIGNATURE;
	fh.Size = width * height * 4;
	fh.BitsOffset = sizeof(BITMAP_HEADER) + sizeof(BITMAP_FILEHEADER);

	h.HeaderSize = sizeof(h);
	h.Planes = 1;
	h.BitCount = 32;
	h.Width = width;
	h.Height = height;
	h.SizeImage = width * height * 4;
	h.PelsPerMeterX = h.PelsPerMeterY = 3780;
	fwrite(&fh, sizeof(fh), 1, f);
	fwrite(&h, sizeof(h), 1, f);
	// flip the image
	for (int i = 0; i < height; ++i)
		fwrite(ptr + (height - i - 1) * width * 4, width * 4, 1, f);
	fclose(f);

	return true;
}
