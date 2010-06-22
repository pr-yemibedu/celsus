#include "stdafx.h"
#include "Logger.hpp"
#include "celsus.hpp"
#include "CelsusExtra.hpp"
#include "path_utils.hpp"

LogMgr* LogMgr::_instance = NULL;

LogMgr::LogMgr() 
  : _file(0)
  , _output_device(Debugger | File)
  , _break_on_error(false)
	, _output_line_numbers(true)
{
  init_severity_map();
}

LogMgr::~LogMgr() 
{
  severity_map_.clear();

  if (_file != NULL)
    fclose(_file);
}

void LogMgr::debug_output( const bool new_line, const bool one_shot, const char *file, const int line, const Severity severity, const char* const format, ...  ) 
{
  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1 + (new_line ? 1 : 0);

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);

  if (new_line) {
    buf[len-2] = '\n';
    buf[len-1] = 0;
  }

  if (one_shot) {
    if (one_shot_set_.end() != one_shot_set_.find(buf))
      return;
    one_shot_set_.insert(buf);
  }

  std::string str = _output_line_numbers ? to_string("%s(%d): %s", file, line, buf) : buf;

  if ((_output_device & LogMgr::Debugger) && severity_map_[LogMgr::Debugger][severity])
    OutputDebugStringA(str.c_str());

  if (_output_device & File) {
    if (!_file) {
      // no output file has been specified, so we use the current module as name
      char buf[MAX_PATH+1];
      buf[MAX_PATH] = 0;
      GetModuleFileNameA(NULL, buf, MAX_PATH);
      open_output_file(Path::replace_extension(buf, "log"));
    }
  }

  if (_file && (_output_device & LogMgr::File) && severity_map_[LogMgr::File][severity]) {
    fputs(str.c_str(), _file);
    fflush(_file);
  }

  va_end(arg);

  if (_break_on_error && severity >= LogMgr::Error) 
    __asm int 3;
}

LogMgr& LogMgr::instance() 
{
  if (_instance == NULL) {
    _instance = new LogMgr();
    atexit(close);
  }
  return *_instance;
}

void LogMgr::close()
{
  SAFE_DELETE(_instance);
}

LogMgr& LogMgr::enable_output(OuputDevice output) 
{
  _output_device |= output;
  return *this;
}

LogMgr& LogMgr::disable_output(OuputDevice output) 
{
  _output_device &= ~output;
  return *this;
}

LogMgr& LogMgr::open_output_file(const char* pFilename) 
{
  // close open file
  if (_file != 0)
    fclose(_file);

  if (0 != fopen_s(&_file, pFilename, "at")) {
    // error opening file
    return *this;
  }

  // write header
  time_t rawTime;
  tm timeInfo;
  char buf[80];
  time(&rawTime);
  if (0 != localtime_s(&timeInfo, &rawTime)) {
    // error
    return *this;
  }
  strftime(buf, 80, "%H:%M:%S (%Y-%m-%d)", &timeInfo);

  fputs("*****************************************************\n", _file);
  fputs("****\n", _file);
  fprintf(_file, "**** Started at: %s\n", buf);
  fputs("****\n", _file);
  fputs("*****************************************************\n", _file);

  fflush(_file);

  return *this;
}

void LogMgr::init_severity_map() 
{
  severity_map_[Debugger][Verbose]  = false;
  severity_map_[Debugger][Info]     = true;
  severity_map_[Debugger][Warning]  = true;
  severity_map_[Debugger][Error]    = true;
  severity_map_[Debugger][Fatal]    = true;

  severity_map_[File][Verbose]  = true;
  severity_map_[File][Info]     = true;
  severity_map_[File][Warning]  = true;
  severity_map_[File][Error]    = true;
  severity_map_[File][Fatal]    = true;
}

LogMgr& LogMgr::enable_severity(const OuputDevice output, const Severity severity) 
{
  severity_map_[output][severity] = true;
  return *this;
}

LogMgr& LogMgr::disable_severity(const OuputDevice output, const Severity severity) 
{
  severity_map_[output][severity] = false;
  return *this;
}

LogMgr& LogMgr::break_on_error(const bool setting) 
{
  _break_on_error = setting;
  return *this;
}

LogMgr& LogMgr::print_file_and_line(const bool value)
{
	_output_line_numbers = value;		
	return *this;
}
