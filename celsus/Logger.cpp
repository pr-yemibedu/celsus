#include "stdafx.h"
#include "Logger.hpp"
#include "celsus.hpp"

std::string toString(char const * const format, ... ) 
{
  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);
  va_end(arg);

  return std::string(buf);
}

LogMgr* LogMgr::instance_ = NULL;

LogMgr::LogMgr() 
  : pFile_(0)
  , output_(LogMgr::Debugger)
  , break_on_error_(false)
{
  InitSeverityMap();
}

LogMgr::~LogMgr() 
{
  close_inner();
}

void LogMgr::DebugState( const bool /*newLine*/, const char* const /*name*/, const char* const /*format*/, ...  ) 
{

}

void LogMgr::DebugOutput( const bool new_line, const bool one_shot, const Severity severity, const char* const format, ...  ) 
{
  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);

  if (one_shot) {
    if (one_shot_set_.end() != one_shot_set_.find(buf)) {
      return;
    }
    one_shot_set_.insert(buf);
  }

  OutputInner(curScope_.c_str(), severity);
  OutputInner(buf, severity);

  if (new_line) {
    OutputInner("\n", severity);
  }
  va_end(arg);

  if (break_on_error_ && severity >= LogMgr::Error) {
    __asm int 3;
  }
}

void LogMgr::OutputInner(const char* str, const Severity severity) 
{
  if (output_ & LogMgr::Debugger && severity_map_[LogMgr::Debugger][severity]) {
    OutputDebugStringA(str);
  }

  if (output_ & LogMgr::File && severity_map_[LogMgr::File][severity]) {
    assert(pFile_ != 0);
    fwrite(str, strlen(str), 1, pFile_);
    fflush(pFile_);
  }
}


LogMgr& LogMgr::instance() 
{
  if (instance_ == NULL) {
    instance_ = new LogMgr();
    atexit(close);
  }
  return *instance_;
}

void LogMgr::close_inner()
{
  instance_->severity_map_.clear();

  if (instance_->pFile_ != NULL) {
    fclose(instance_->pFile_);
    instance_->pFile_ = NULL;
  }
}

void LogMgr::close()
{
  SAFE_DELETE(instance_);
}

void LogMgr::EnterScope(const std::string& str) 
{
  scopeStack_.push_back(str);
  RecalcScope();
}

void LogMgr::LeaveScope() 
{
  scopeStack_.pop_back();
  RecalcScope();
}

void LogMgr::RecalcScope() 
{
  curScope_ = "";
  for (ScopeStack::iterator it = scopeStack_.begin(); it != scopeStack_.end(); ++it)
  {
    curScope_ += *it;
    curScope_ += ".";
  }

}

LogMgr& LogMgr::EnableOutput(Output output) 
{
  output_ = static_cast<Output>(output_ | output);
  return *this;
}

LogMgr& LogMgr::DisableOutput(Output output) 
{
  output_ = static_cast<Output>(output_ & ~output);
  return *this;
}

LogMgr& LogMgr::OpenOutputFile(const char* pFilename) 
{
  // close open file
  if (pFile_ != 0) {
    fclose(pFile_);
  }

  if (0 != fopen_s(&pFile_, pFilename, "at")) {
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

  fputs("*****************************************************\n", pFile_);
  fputs("****\n", pFile_);
  fprintf(pFile_, "**** Started at: %s\n", buf);
  fputs("****\n", pFile_);
  fputs("*****************************************************\n", pFile_);

  fflush(pFile_);

  return *this;
}

void LogMgr::InitSeverityMap() 
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

LogMgr& LogMgr::EnableSeverity(const Output output, const Severity severity) 
{
  severity_map_[output][severity] = true;
  return *this;
}

LogMgr& LogMgr::DisableSeverity(const Output output, const Severity severity) 
{
  severity_map_[output][severity] = false;
  return *this;
}

LogMgr::Scope::Scope(const std::string& str) 
{
  LOG_MGR.EnterScope(str);
}

LogMgr::Scope::~Scope() 
{
  LOG_MGR.LeaveScope();
}

std::string LogMgr::ToString(char const * const format, ... ) 
{
  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);
  va_end(arg);

  return std::string(buf);
}

LogMgr& LogMgr::break_on_error(const bool setting) 
{
  break_on_error_ = setting;
  return *this;
}
