#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <deque>
#include <map>
#include <set>
#include <windows.h>

/**
 * The Logger is designed to be a stand alone, drop in logging component. The class itself
 * is a singleton (probably the only true case where I can justify singletons), and logging is
 * done via the LOG_xxx family of macros below
 */

// TODO: add some kind of global filter that we can toggle on/off (grep/regex)

#define LOG_MGR LogMgr::instance()

#define LOG_VERBOSE(fmt, ...) LOG_MGR.debug_output(false, false, __FILE__, __LINE__, LogMgr::Verbose, fmt, __VA_ARGS__ );
#define LOG_VERBOSE_LN(fmt, ...) LOG_MGR.debug_output(true, false, __FILE__, __LINE__, LogMgr::Verbose, fmt, __VA_ARGS__ );
#define LOG_INFO(fmt, ...) LOG_MGR.debug_output(false, false, __FILE__, __LINE__, LogMgr::Info, fmt, __VA_ARGS__ );
#define LOG_INFO_LN(fmt, ...) LOG_MGR.debug_output(true, false, __FILE__, __LINE__, LogMgr::Info, fmt, __VA_ARGS__ );
#define LOG_WARNING(fmt, ...) LOG_MGR.debug_output(false, false, __FILE__, __LINE__, LogMgr::Warning, fmt, __VA_ARGS__ );
#define LOG_WARNING_LN(fmt, ...) LOG_MGR.debug_output(true, false, __FILE__, __LINE__, LogMgr::Warning, fmt, __VA_ARGS__ );
#define LOG_WARNING_LN_ONESHOT(fmt, ...) LOG_MGR.debug_output(true, true, __FILE__, __LINE__, LogMgr::Warning, fmt, __VA_ARGS__ );
#define LOG_ERROR(fmt, ...) LOG_MGR.debug_output(false, false, __FILE__, __LINE__, LogMgr::Error, fmt, __VA_ARGS__ );
#define LOG_ERROR_LN(fmt, ...) LOG_MGR.debug_output(true, false, __FILE__, __LINE__, LogMgr::Error, fmt, __VA_ARGS__ );
#define LOG_ERROR_LN_ONESHOT(fmt, ...) LOG_MGR.debug_output(true, true, __FILE__, __LINE__, LogMgr::Error, fmt, __VA_ARGS__ );
#define LOG_FATAL(fmt, ...) LOG_MGR.debug_output(false, false, __FILE__, __LINE__, LogMgr::Fatal, fmt, __VA_ARGS__ );
#define LOG_FATAL_LN(fmt, ...) LOG_MGR.debug_output(true, false, __FILE__, __LINE__, LogMgr::Fatal, fmt, __VA_ARGS__ );

class LogMgr 
{
public:
  enum OuputDevice
  {
    Debugger    = (1 << 0),
    File        = (1 << 1),
  };

  enum Severity 
  {
    Verbose     = (1 << 0),
    Info        = (1 << 1),
    Warning     = (1 << 2),
    Error       = (1 << 3),
    Fatal       = (1 << 4),
  };

  void debug_output(const bool newLine, const bool one_shot, const char *file, const int line, const Severity severity, const char* const format, ...  );

  LogMgr& enable_output(OuputDevice output);
  LogMgr& disable_output(OuputDevice output);
  LogMgr& open_output_file(const char* pFilename);
  LogMgr& break_on_error(const bool setting);

  LogMgr& enable_severity(const OuputDevice output, const Severity severity);
  LogMgr& disable_severity(const OuputDevice output, const Severity severity);

  static LogMgr& instance();
  static void close();

private:
  LogMgr();
  ~LogMgr();

  void    init_severity_map();

  int   output_device_;
  FILE* file_;
  bool break_on_error_;

  std::set<std::string> one_shot_set_;
  std::map<OuputDevice, std::map<Severity, bool> > severity_map_;
  static LogMgr* instance_;
};

#endif
