#include "stdafx.h"
#include "text_scanner.hpp"
#include "file_utils.hpp"

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
  return ch >= '0' && ch <= '9';
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

const char *scan_read_line2(const char *buf, const char *buf_end, int *len)
{
	const char *e = buf;
	while (e <= buf_end && !is_newline(*e))
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

const char *skip_chars(const char *buf, const char *buf_end, const char *tokens)
{
	bool found = true;
	while (*buf && buf <= buf_end && found) {
		found = false;
		for (int i = 0; tokens[i] && !found; ++i) {
			if (tokens[i] == *buf)
				found = true;
		}
		if (!found)
			break;
		++buf;
	}
	return buf;
}


const char *parse_floats(const char *buf, const char *buf_end, std::vector<float>* out)
{
  float f;
	const char *tmp = NULL;
  while (buf && (buf = scan_parse_float(buf, buf_end, &f)) != NULL && (tmp = buf) != NULL) {
    out->push_back(f);
    buf = skip_chars(buf, buf_end, ", \t");
  }
	return tmp;
}

const char *parse_ints(const char *buf, const char *buf_end, std::vector<int>* out)
{
  int i;
	const char *tmp = NULL;
  while (buf && (buf = scan_parse_int(buf, buf_end, &i)) != NULL && (tmp = buf) != NULL) {
    out->push_back(i);
    buf = skip_chars(buf, buf_end, ", \t");
  }
	return tmp;
}


TextScanner::TextScanner()
	: _buf(NULL)
	, _buf_end(NULL)
	, _cur(NULL)
	, _prev(NULL)
	, _len(0)
{

}

bool TextScanner::load(const char *filename)
{
	_buf = _cur = _prev = (char *)load_file(filename, &_len);
	if (!_buf)
		return false;
	_buf_end = _buf + _len - 1;
	return true;
}

void TextScanner::reset()
{
	_cur = _prev = _buf;
}

bool TextScanner::read_float(float *res)
{
	if (!_buf)
		return false;

	_prev = _cur;
	const char *tmp = scan_parse_float(_cur, _buf_end, res);
	if (!tmp)
		return false;

	// skip trailing whitespace
	_cur = ::skip_chars(tmp, _buf_end, ", \t");
	return true;
}

bool TextScanner::skip_line()
{
	return true;
}

bool TextScanner::read_line(const char **res, int *len)
{
	if (!_cur || _cur > _buf_end)
		return false;

	_prev = _cur;

	if ((*res = scan_read_line2(_cur, _buf_end, len)) == NULL)
		return false;

	skip_to_next_line();

	return true;
}

bool TextScanner::skip_to_next_line()
{
	if (eof())
		return false;

	while (*_cur && _cur <= _buf_end && !(*_cur == '\r' || *_cur == '\n'))
		_cur++;

	bool skipped = false;
	while (*_cur && _cur <= _buf_end && (*_cur == '\r' || *_cur == '\n')) {
		skipped = true;
		_cur++;
	}
	return skipped;
}

bool TextScanner::skip_chars_lenient(const char *tokens)
{
	if (eof())
		return false;
	skip_chars(tokens);
	return true;
}

bool TextScanner::skip_chars(const char *tokens)
{
	if (eof())
		return false;

	bool found = true;
	bool found_any = false;
	while (*_cur && _cur <= _buf_end && found) {
		found = false;
		// check if the current char is one of the skip tokens
		for (int i = 0; tokens[i] && !found; ++i) {
			if (tokens[i] == *_cur)
				found = true;
		}
		found_any |= found;
		if (!found)
			break;
		++_cur;
	}
	return found_any;
}

bool TextScanner::read_floats(std::vector<float> *out)
{
	out->clear();
	if (eof())
		return false;

	_prev = _cur;

	_cur = parse_floats(_cur, _buf_end, out);
	return !out->empty();
}

bool TextScanner::read_ints(std::vector<int> *out)
{
	out->clear();
	if (eof())
		return false;

	_prev = _cur;

	if (const char *tmp = ::parse_ints(_cur, _buf_end, out))
		_cur = tmp;

	return !out->empty();
}

bool TextScanner::read_int(int *out)
{
	if (eof())
		return false;

	_prev = _cur;

	const char *tmp = scan_parse_int(_cur, _buf_end, out);
	if (!tmp)
		return false;

	// skip trailing whitespace
	_cur = ::skip_chars(tmp, _buf_end, ", \t");
	return true;
}

void TextScanner::rewind()
{
	_cur = _prev;
}

bool TextScanner::eof() const
{
	return !_cur || _cur > _buf_end;
}

bool TextScanner::peek(char *res, const int count)
{
	// 012
	// xx0  
	// ^ ^
	// | +-- buf_end
	// +---- cur
	if (_buf_end - _cur < count)
		return false;
	memcpy(res, _cur, count);
	return true;
}

bool TextScanner::peek(char *res)
{
	if (eof())
		return false;
	*res = *_cur;
	return true;
}

bool TextScanner::read_string(string2 *out)
{
	if (eof())
		return false;
	_prev = _cur;
	while (!(is_newline(*_cur) || is_whitespace(*_cur) || eof()))
		++_cur;

	out->assign(_prev, _cur - _prev);

	// skip trailing whitespace
	_cur = ::skip_chars(_cur, _buf_end, " \t");
  return true;
}
