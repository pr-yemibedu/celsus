#include "stdafx.h"
#include "text_scanner.hpp"

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
