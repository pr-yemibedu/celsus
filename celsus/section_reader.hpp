#pragma once

class SectionReader
{
public:
  SectionReader();
  ~SectionReader();

  bool load(const char *filename, const char *section = NULL);
  bool load_section(const char *section);

  bool read_vector3(D3DXVECTOR3 *out);
  bool read_color(D3DXCOLOR *out);
  bool read_float(float *out);
  bool read_int(int *out);
  bool read_string(std::string *out);
private:

  typedef std::vector<std::string> Section;
  typedef std::map<std::string, Section> Sections;

  bool init_cur_section(const char **buf, const char **buf_end);
  bool read_floats_inner(int count, float *out);
  bool read_ints_inner(int count, int *out);

  char *_buf;
  int _buf_len;

  Section* _cur_section;
  int _section_ofs;

  Sections _sections;
};
