#pragma once

const char *scan_find_next(const char *buf, const char *buf_end);
const char *scan_read_line(const char *buf, const char *buf_end, int *len);
const char *scan_skip_char(const char *buf, const char *buf_end, char ch);
const char *scan_parse_int(const char *buf, const char *buf_end, int *i);
const char *scan_parse_float(const char *buf, const char *buf_end, float *f);
const char *scan_skip_line(const char *buf, const char *buf_end);
const char *scan_find(const char *buf, const char *buf_end, const char ch);
const char *scan_get_between(const char *buf, const char *buf_end, const char start, const char end, int *len);
void parse_floats(const char *buf, const char *buf_end, std::vector<float>* out);
void parse_ints(const char *buf, const char *buf_end, std::vector<int>* out);


