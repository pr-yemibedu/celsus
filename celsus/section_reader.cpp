#include "stdafx.h"
#include "celsus.hpp"
#include "section_reader.hpp"

namespace
{
  bool is_whitespace(char ch)
  {
    return ch == ' ' || ch == '\t';
  }

  bool is_newline(char ch)
  {
    return ch == '\r' || ch == '\n';
  }

  bool is_digit(const char ch)
  {
    return ch >= '0' && ch < '9';
  }

  // find the next parsable char
  // skip # to the end of the row, and skips \r\n\t 
  const char *scan_find_next(const char *buf, const char *buf_end)
  {
    while (++buf <= buf_end && (is_whitespace(*buf) || is_newline(*buf) || *buf == '#')) {
      // check for comment start
      const char ch = *buf;
      if (ch == '#') {
        // skip till the end of the row
        while (buf <= buf_end && !is_newline(*buf))
          buf++;
        while (buf <= buf_end && is_newline(*buf))
          buf++;
      }
    }
    return buf > buf_end ? NULL : buf;
  }

  const char *scan_read_line(const char *buf, const char *buf_end, int *len)
  {
    const char *e = buf;
    while (e <= buf_end && !is_newline(*e) && *e != '#')
      ++e;
    if (len) *len = e - buf;
    return buf;
  }

  const char *scan_skip_char(const char *buf, const char *buf_end, char ch)
  {
    while (buf <= buf_end && *buf++ != ch)
      ;
    return buf > buf_end ? NULL : buf;
  }

  const char *scan_parse_int(const char *buf, const char *buf_end, int *i)
  {
    // skip any leading whitespace
    while (buf <= buf_end && is_whitespace(*buf))
      ++buf;

    if (buf > buf_end)
      return NULL;

    // check for a sign
    int neg = 1;
    switch (*buf) {
    case '-':
      neg = -1;
    case '+':
      ++buf;
    }

    int res = 0;
    bool ok = false;
    while (buf <= buf_end && is_digit(*buf)) {
      res = res * 10 + (*buf - '0');
      ++buf;
      ok = true;
    }

    *i = neg * res;
    return ok ? buf : NULL;
  }

  const char *scan_parse_float(const char *buf, const char *buf_end, float *f)
  {
    // skip any leading whitespace
    while (buf <= buf_end && is_whitespace(*buf))
      ++buf;

    if (buf > buf_end)
      return NULL;

    // check for a sign
    float neg = 1;
    switch (*buf) {
    case '-':
      neg = -1;
    case '+':
      ++buf;
    }

    // parse digits before .
    float res = 0;
    bool ok = false;
    while (buf <= buf_end && is_digit(*buf)) {
      res = res * 10 + (*buf - '0');
      ++buf;
      ok = true;
    }

    // skip .
    if (buf <= buf_end && *buf == '.')
      ++buf;

    float frac_mul = 0.1f;

    // parse fractional part
    while (buf <= buf_end && is_digit(*buf)) {
      res += (*buf - '0') * frac_mul;
      frac_mul /= 10;
      buf++;
      ok = true;
    }

    *f = neg * res;
    return ok ? buf : NULL;
  }

  const char *scan_skip_line(const char *buf, const char *buf_end)
  {
    // skip till the end of the row
    while (buf <= buf_end && !is_newline(*buf))
      ++buf;
    while (buf <= buf_end && is_newline(*buf))
      ++buf;
    return buf > buf_end ? NULL : buf;
  }

  const char *scan_find(const char *buf, const char *buf_end, const char ch)
  {
    while (buf <= buf_end && buf) {
      if (*buf == ch)
        break;
      buf = scan_find_next(buf, buf_end);
    }
    return buf;
  }

  const char *scan_get_between(const char *buf, const char *buf_end, const char start, const char end, int *len)
  {
    // 01234567
    // [MAGNUS].. s = [, e = ]
    const char *s = scan_find(buf, buf_end, start);
    if (!s) return NULL;

    const char *e = scan_find(s, buf_end, end);
    if (!e) return NULL;

    *len = e - s - 1;
    return *len == 0 ? NULL : s + 1;
  }

  const char *is_section_header(const char *buf, const char *buf_end, int *len)
  {
    return scan_get_between(buf, buf_end, '[', ']', len);
  }

  void parse_floats(const char *buf, const char *buf_end, std::vector<float>* out)
  {
    float f;
    while (buf && (buf = scan_parse_float(buf, buf_end, &f)) != NULL) {
      out->push_back(f);
      buf = scan_skip_char(buf, buf_end, ',');
    }
  }

  void parse_ints(const char *buf, const char *buf_end, std::vector<int>* out)
  {
    int i;
    while (buf && (buf = scan_parse_int(buf, buf_end, &i)) != NULL) {
      out->push_back(i);
      buf = scan_skip_char(buf, buf_end, ',');
    }
  }

}

SectionReader::SectionReader()
  : _cur_section(NULL)
  , _buf(NULL)
  , _buf_len(0)
  , _section_ofs(0)
{
}

SectionReader::~SectionReader()
{
  SAFE_ADELETE(_buf);
}

bool SectionReader::load(const char *filename, const char *section)
{
  SAFE_ADELETE(_buf);

  HANDLE h = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
  if (h == INVALID_HANDLE_VALUE)
    return false;

  DWORD hi, _buf_len = GetFileSize(h, &hi);
  _buf = new char[_buf_len];
  DWORD bytes_read;
  if (!ReadFile(h, (void*)_buf, _buf_len, &bytes_read, NULL))
    return false;
  CloseHandle(h);

  // load all the sections
  const char *cur = _buf;
  const char *buf_end = _buf + _buf_len;
  std::string section_header;
  std::vector< std::string > rows;
  int len = 0;
  while (cur && (cur = scan_read_line(cur, buf_end, &len)) != NULL) {
    if (len > 0) {
      int len2 = 0;
      if (const char *h = is_section_header(cur, cur + len, &len2)) {
        // store the previous section header
        if (!rows.empty()) {
          _sections.insert(std::make_pair(section_header, rows));
          rows.clear();
        }
        section_header = std::string(h, len2);
      } else {
        rows.push_back(std::string(cur, len));
      }
    }
    cur = scan_skip_line(cur, buf_end);
  }

  if (!rows.empty())
    _sections.insert(std::make_pair(section_header, rows));

  bool res = true;
  if (section)
    res &= load_section(section);

  return res;
}

bool SectionReader::load_section(const char *section)
{
  _cur_section = NULL;
  _section_ofs = 0;

  auto it = _sections.find(section);
  if (it == _sections.end())
    return false;

  _cur_section = &(it->second);
  return true;
}


bool SectionReader::init_cur_section(const char **buf, const char **buf_end)
{
  if (!_cur_section || _section_ofs >= (int)_cur_section->size())
    return false;

  *buf = (*_cur_section)[_section_ofs].c_str();
  *buf_end = *buf + (*_cur_section)[_section_ofs].size();
  _section_ofs++;
  return true;
}

bool SectionReader::read_floats_inner(int count, float* out)
{
  const char *buf, *buf_end;
  if (!init_cur_section(&buf, &buf_end))
    return false;

  std::vector<float> f;
  parse_floats(buf, buf_end, &f);
  if ((int)f.size() != count)
    return false;

  for (int i = 0; i < count; ++i)
    *out++ = f[i];

  return true;
}

bool SectionReader::read_ints_inner(int count, int *out)
{
  const char *buf, *buf_end;
  if (!init_cur_section(&buf, &buf_end))
    return false;

  std::vector<int> v;
  parse_ints(buf, buf_end, &v);
  if ((int)v.size() != count)
    return false;

  for (int i = 0; i < count; ++i)
    *out++ = v[i];

  return true;
}

bool SectionReader::read_string(std::string *out)
{
  const char *buf, *buf_end;
  if (!init_cur_section(&buf, &buf_end))
    return false;

  int len;
  if (!scan_read_line(buf, buf_end, &len))
    return false;
  *out = std::string(buf, len);

  return true;
}

bool SectionReader::read_float(float *out)
{
  return read_floats_inner(1, out);
}

bool SectionReader::read_int(int *out)
{
  return read_ints_inner(1, out);
}

bool SectionReader::read_vector3(D3DXVECTOR3 *out)
{
  return read_floats_inner(3, &out->x);
}

bool SectionReader::read_color(D3DXCOLOR *out)
{
  return read_floats_inner(4, &out->r);
}
