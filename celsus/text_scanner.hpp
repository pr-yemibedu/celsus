#pragma once
#include "string_utils.hpp"

const char *scan_read_line(const char *buf, const char *buf_end, int *len);
const char *scan_skip_char(const char *buf, const char *buf_end, char ch);
const char *scan_parse_int(const char *buf, const char *buf_end, int *i);
const char *scan_parse_float(const char *buf, const char *buf_end, float *f);
const char *scan_skip_line(const char *buf, const char *buf_end);
const char *scan_find(const char *buf, const char *buf_end, const char ch);
const char *scan_get_between(const char *buf, const char *buf_end, const char start, const char end, int *len);
const char *parse_floats(const char *buf, const char *buf_end, std::vector<float>* out);
const char *parse_ints(const char *buf, const char *buf_end, std::vector<int>* out);

class TextScanner
{
public:
	TextScanner();
	bool load(const char *filename);

  TextScanner& set_line_mode(const bool line_mode);

	bool peek(char *res);
	bool peek(char *res, const int count);
	bool read_float(float *res);
  bool read_floats(std::vector<float>* out);
  bool read_floats(float *out, int *count);
  bool read_hex(uint32_t *out);
	bool read_int(int *out);
  bool read_ints(std::vector<int>* out);
  bool read_ints(int *out, int *count);
	bool read_line(const char **res, int *len);
	bool read_line(string2 *out);
	bool read_string(string2 *out);

	bool skip_line();
	bool skip_chars(const char *tokens);
	// returns false iff scanner is eof
	bool skip_chars_lenient(const char *tokens);
	bool skip_to_next_line();

  // skip empty lines, and lines starting with a #-sign
  bool skip_dummy_lines();

	bool eof() const;

	void rewind();
	void reset();

  int buf_size() const { return _buf_end - _buf; }

private:
	const char *_buf;
	const char *_buf_end;
	const char *_cur;
	const char *_prev;
	uint32_t _len;
  bool _line_mode;    // each call to the read functions is preceeded by a call to skip_dummy_lines
};