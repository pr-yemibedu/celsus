#pragma once

#include <stdint.h>
#include <string>
#include <stack>

#define CHUNK_SUPPORTS_BZLIB
#define CHUNK_SUPPORTS_ZLIB

#ifdef CHUNK_SUPPORTS_BZLIB
#include <bzlib.h>
#endif
#ifdef CHUNK_SUPPORTS_ZLIB
#include <zlib.h>
#endif

// TODO: Make this a template class, taking the id enum as the template parameter, and move it to celsus

#pragma pack(push, 1)
struct ChunkHeader 
{
  enum Id {
    Info,
    Geometry,
    Mesh,
    Camera,
    Light,
    Hierarchy,
    Animation,
    Transform,
  };

  ChunkHeader(const Id id) : id_(id), size_(0) {}
  ChunkHeader(const Id id, const uint32_t size) : id_(id), size_(size) {}

  Id        id_;
  uint32_t  size_;
};
#pragma pack(pop)

class ChunkIo
{
public:
#pragma pack(push, 1)
  struct MainHeader
  {
    enum Version
    {
      Uncompressed,
      CompressedBZLib,
      CompressedZLib,
    };

    static const uint32_t kHeaderId = 'DATA';

    uint32_t  id;
    Version   version;
    uint32_t  uncompressesd_size;
  };
#pragma pack(pop)

  ChunkIo();
  ~ChunkIo();

  // reader
  bool          init_reader(uint8_t* data, const uint32_t data_len);
  bool          is_eof();
  bool          is_end_of_chunk();
  ChunkHeader   cur_header();
  int32_t       read_int();
  uint32_t      read_uint();
  std::string   read_string();
  char*         read_cstring();
  uint8_t*      read_data(const uint32_t len);
  bool          next();

  template<typename T>
  T read_generic() {
    const T value = *reinterpret_cast<T*>(data_ptr());
    cur_data_pos_ += sizeof(T);
    return value;
  }

  // writer
  bool  init_writer(const MainHeader::Version version);
  void  enter_scope(const ChunkHeader::Id id);
  void  leave_scope(const ChunkHeader::Id id);
  void  get_buffer(uint8_t*& buf, uint32_t& len);
  void  end_of_data();
  bool  write_raw_data(const uint8_t* data, const uint32_t len);
  bool  write_string(const std::string& str);
  template<typename T> bool write_generic(const T value) 
  {
    const uint32_t len = sizeof(T);
    if (!expand_buffer_if_needed(len)) {
      return false;
    }
    return write_raw_data((const uint8_t*)&value, len);
  }

private:
  uint8_t*      data_ptr();
  bool          handle_compression();

  bool  expand_buffer_if_needed(const uint32_t data_size);

  uint8_t* reader_data_;
  uint32_t reader_data_len_;
  uint32_t last_header_pos_;
  uint32_t cur_data_pos_;

  static const uint32_t kDefaultBufLen = 32 * 1024;
  uint8_t* writer_buf_;
  uint32_t writer_buf_len_;
  uint32_t bytes_used_;
  MainHeader::Version version_;
  std::stack<long> header_pos_stack_;

};

class ChunkScoper
{
public:
  ChunkScoper(ChunkIo& writer, const ChunkHeader::Id id) : writer_(writer), id_(id) { writer_.enter_scope(id); }
  ~ChunkScoper() { writer_.leave_scope(id_); }
private:
  ChunkIo& writer_;
  const ChunkHeader::Id id_;
};

#define GEN_NAME2(prefix, line) prefix##line
#define GEN_NAME(prefix, line) GEN_NAME2(prefix, line)
#define SCOPED_CHUNK(writer, id) ChunkScoper GEN_NAME(chunk, __LINE__)(writer, id)
