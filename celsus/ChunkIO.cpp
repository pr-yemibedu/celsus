#include "StdAfx.h"
#include "ChunkIO.hpp"
#include "ErrorHandling.hpp"
#include "celsus.hpp"

ChunkIo::ChunkIo() 
  : reader_data_(NULL)
  , reader_data_len_(0)
  , cur_data_pos_(0)
  , last_header_pos_(0)
  , writer_buf_(NULL)
  , writer_buf_len_(0)
  , bytes_used_(0)
{
}

ChunkIo::~ChunkIo() 
{
  SAFE_ADELETE(reader_data_);
  reader_data_len_ = 0;

  SAFE_ADELETE(writer_buf_);
  writer_buf_len_ = 0;
}

bool ChunkIo::init_reader(uint8_t* data, const uint32_t data_len)
{
  if (data == NULL || data_len == 0) {
    return false;
  }
  reader_data_ = data;
  reader_data_len_ = data_len;
  cur_data_pos_ = 0;
  last_header_pos_ = 0;
  if (!handle_compression()) {
    return false;
  }
  return true;
}

bool ChunkIo::handle_compression()
{
  MainHeader* main_header = (MainHeader*)reader_data_;
  if (main_header->id != MainHeader::kHeaderId) {
    LOG_WARNING_LN("Invalid header");
    return false;
  }
  reader_data_len_ = main_header->uncompressesd_size;

  switch (main_header->version) {
    case MainHeader::Uncompressed:
      reader_data_ += sizeof(MainHeader);
      break;

    case MainHeader::CompressedBZLib:
      {
#ifdef CHUNK_SUPPORTS_BZLIB
        uint8_t* uncompressed_data = new uint8_t[main_header->uncompressesd_size];
        uint32_t dest_len = main_header->uncompressesd_size;
        const int32_t res = BZ2_bzBuffToBuffDecompress(
          (char*)uncompressed_data, &dest_len, (char*)(reader_data_ + sizeof(MainHeader)), reader_data_len_ - sizeof(MainHeader), 0, 0);
        if (res != BZ_OK) {
          LOG_WARNING_LN("Error decompressing data");
          return false;
        }
        delete [] reader_data_;
        reader_data_ = uncompressed_data;
#else
        throw std::string("bzlib not supported");
#endif
      }
      break;

    case MainHeader::CompressedZLib:
      {
#ifdef CHUNK_SUPPORTS_ZLIB
        uint8_t* uncompressed_data = new uint8_t[main_header->uncompressesd_size];
        uLongf dest_len = main_header->uncompressesd_size;
        const int32_t res = uncompress(
          (Bytef*)uncompressed_data, &dest_len, (Bytef*)(reader_data_ + sizeof(MainHeader)), reader_data_len_ - sizeof(MainHeader));
        if (res != BZ_OK) {
          LOG_WARNING_LN("Error decompressing data");
          return false;
        }
        delete [] reader_data_;
        reader_data_ = uncompressed_data;
#else
        throw std::string("zlib not supported");
#endif

      }
      break;
  }
  return true;
}

bool ChunkIo::is_eof() 
{
  return cur_data_pos_ >= reader_data_len_;
}

bool ChunkIo::is_end_of_chunk() 
{
  return cur_data_pos_ - last_header_pos_ >= cur_header().size_;
}

ChunkHeader ChunkIo::cur_header() 
{
  return *reinterpret_cast<ChunkHeader*>(&reader_data_[last_header_pos_]);
}

uint8_t* ChunkIo::data_ptr() 
{
  if (is_eof()) {
    return NULL;
  }
  return reinterpret_cast<uint8_t*>(&reader_data_[cur_data_pos_ + sizeof(ChunkHeader)]);
}

bool ChunkIo::next() 
{
  if (is_eof()) {
    return false;
  }
  last_header_pos_ += cur_header().size_ + sizeof(ChunkHeader);
  cur_data_pos_ = last_header_pos_;
  return true;
}

uint32_t ChunkIo::read_uint() 
{
  return read_generic<uint32_t>();
}

int32_t ChunkIo::read_int() 
{
  return read_generic<int32_t>();
}

std::string ChunkIo::read_string() 
{
  const uint32_t string_len = read_int();
  return std::string(reinterpret_cast<char*>(read_data(string_len)), string_len);
}

char* ChunkIo::read_cstring() 
{
  const uint32_t string_len = read_int();
  return reinterpret_cast<char*>(read_data(string_len));
}

uint8_t* ChunkIo::read_data(const uint32_t len) 
{
  uint8_t* ptr = data_ptr();
  cur_data_pos_ += len;
  return ptr;
}



bool ChunkIo::init_writer(const MainHeader::Version version)
{
  writer_buf_ = new uint8_t[kDefaultBufLen];
  writer_buf_len_ = kDefaultBufLen;
  bytes_used_ = 0;
  version_ = version;

  MainHeader header;
  header.id = MainHeader::kHeaderId;
  header.version = version_;
  if (!write_generic(header)) {
    return false;
  }
  return true;
}


bool ChunkIo::expand_buffer_if_needed(const uint32_t data_size)
{
  if (bytes_used_ + data_size <= writer_buf_len_) {
    return true;
  }
  const uint32_t new_size = 2 * max(writer_buf_len_, data_size);
  uint8_t* new_buf = new uint8_t[new_size];
  if (new_buf == NULL) {
    return false;
  }
  memcpy(new_buf, writer_buf_, bytes_used_);
  delete [] writer_buf_;
  writer_buf_ = new_buf;
  writer_buf_len_ = new_size;
  return true;
}

bool ChunkIo::write_raw_data(const uint8_t* data, const uint32_t len) 
{
  if (!expand_buffer_if_needed(len)) {
    return false;
  }

  memcpy(&writer_buf_[bytes_used_], data, len);
  bytes_used_ += len;

  return true;
}


// Strings are written as [len, data]
bool ChunkIo::write_string(const std::string& str)
{
  const size_t len = str.length();
  if (!write_generic<int32_t>(len)) {
    return false;
  }

  if (!write_raw_data((const uint8_t*)&str.c_str()[0], len)) {
    return false;
  }
  return true;
}


void ChunkIo::get_buffer(uint8_t*& buf, uint32_t& len)
{
  buf = writer_buf_;
  len = bytes_used_;
}

void ChunkIo::enter_scope(const ChunkHeader::Id id)
{
  // Save header pos, and write header with dummy info
  const long cur_pos = bytes_used_;
  header_pos_stack_.push(cur_pos);
  ChunkHeader header(id);
  write_generic(header);
}

void ChunkIo::leave_scope(const ChunkHeader::Id id)
{
  // Save current pos, jump back to header start, write the correct block length, and jump back
  const long end_of_data = bytes_used_;
  const long last_header_pos = header_pos_stack_.top();
  header_pos_stack_.pop();
  const uint32_t data_length = (end_of_data - last_header_pos) - sizeof(ChunkHeader);
  bytes_used_ = last_header_pos;
  ChunkHeader header(id, data_length);
  write_generic(header);
  bytes_used_ = end_of_data;
}

void ChunkIo::end_of_data()
{
  MainHeader* header = (MainHeader*)writer_buf_;
  header->uncompressesd_size = bytes_used_ - sizeof(MainHeader);

  switch (header->version) {
    case MainHeader::Uncompressed:
      {
      }
      break;

    case MainHeader::CompressedBZLib:
      {
#ifdef CHUNK_SUPPORTS_BZLIB
        // from the bzip manual
        uint32_t compressed_buf_len = 101 * bytes_used_ / 100 + 600;
        uint8_t* compressed_buf = new uint8_t[compressed_buf_len];
        memcpy(compressed_buf, writer_buf_, sizeof(MainHeader));
        uint32_t dest_len = compressed_buf_len;
        const int32_t res = BZ2_bzBuffToBuffCompress(
          (char*)(compressed_buf + sizeof(MainHeader)), &dest_len, 
          (char*)(writer_buf_ + sizeof(MainHeader)), bytes_used_ - sizeof(MainHeader), 9, 0, 0);
        if (res != BZ_OK) {
          throw std::string("Error compressing data");
        }
        printf("Data compressed: [%d -> %d]\n", bytes_used_ - sizeof(MainHeader), dest_len);
        delete [] writer_buf_;
        writer_buf_ = compressed_buf;
        bytes_used_ = dest_len + sizeof(MainHeader);
#else
        throw std::exception("bzlib not supported");
#endif
      }
      break;

    case MainHeader::CompressedZLib:
      {
#ifdef CHUNK_SUPPORTS_ZLIB
        // from the zlib manual
        uint32_t compressed_buf_len = 101 * bytes_used_ / 100 + 12;
        uint8_t* compressed_buf = new uint8_t[compressed_buf_len];
        memcpy(compressed_buf, writer_buf_, sizeof(MainHeader));
        uLongf dest_len = compressed_buf_len;
        const int32_t res = compress2(
          (Bytef*)(compressed_buf + sizeof(MainHeader)), &dest_len, 
          (Bytef*)(writer_buf_ + sizeof(MainHeader)), bytes_used_ - sizeof(MainHeader), 9);
        if (res != Z_OK) {
          throw std::string("Error compressing data");
        }
        printf("Data compressed: [%d -> %d]\n", bytes_used_ - sizeof(MainHeader), dest_len);
        delete [] writer_buf_;
        writer_buf_ = compressed_buf;
        bytes_used_ = dest_len + sizeof(MainHeader);
#else
        throw std::exception("zlib not supported");
#endif
      }
      break;
  }
}
