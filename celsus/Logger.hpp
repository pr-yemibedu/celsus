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

#define LOG_SCOPE(fmt, ...)  LogMgr::Scope GEN_NAME(scope, __LINE__)(LogMgr::ToString(fmt, __VA_ARGS__));
#define LOG_STATE(name, fmt, ...) LOG_MGR.DebugState(false, false, name, fmt, __VA_ARGS__ );
#define LOG_VERBOSE(fmt, ...) LOG_MGR.DebugOutput(false, false, LogMgr::Verbose, fmt, __VA_ARGS__ );
#define LOG_VERBOSE_LN(fmt, ...) LOG_MGR.DebugOutput(true, false, LogMgr::Verbose, fmt, __VA_ARGS__ );
#define LOG_INFO(fmt, ...) LOG_MGR.DebugOutput(false, false, LogMgr::Info, fmt, __VA_ARGS__ );
#define LOG_INFO_LN(fmt, ...) LOG_MGR.DebugOutput(true, false, LogMgr::Info, fmt, __VA_ARGS__ );
#define LOG_WARNING(fmt, ...) LOG_MGR.DebugOutput(false, false, LogMgr::Warning, fmt, __VA_ARGS__ );
#define LOG_WARNING_LN(fmt, ...) LOG_MGR.DebugOutput(true, false, LogMgr::Warning, fmt, __VA_ARGS__ );
#define LOG_WARNING_LN_ONESHOT(fmt, ...) LOG_MGR.DebugOutput(true, true, LogMgr::Warning, fmt, __VA_ARGS__ );
#define LOG_ERROR(fmt, ...) LOG_MGR.DebugOutput(false, false, LogMgr::Error, fmt, __VA_ARGS__ );
#define LOG_ERROR_LN(fmt, ...) LOG_MGR.DebugOutput(true, false, LogMgr::Error, fmt, __VA_ARGS__ );
#define LOG_ERROR_LN_ONESHOT(fmt, ...) LOG_MGR.DebugOutput(true, true, LogMgr::Error, fmt, __VA_ARGS__ );
#define LOG_FATAL(fmt, ...) LOG_MGR.DebugOutput(false, false, LogMgr::Fatal, fmt, __VA_ARGS__ );
#define LOG_FATAL_LN(fmt, ...) LOG_MGR.DebugOutput(true, false, LogMgr::Fatal, fmt, __VA_ARGS__ );

class LogMgr {
public:
  enum Output {
    Debugger    = (1 << 0),
    File        = (1 << 1),
  };

  enum Severity {
    Verbose     = (1 << 0),
    Info        = (1 << 1),
    Warning     = (1 << 2),
    Error       = (1 << 3),
    Fatal       = (1 << 4),
  };

  /**
   * Output the named state
   */
  void DebugState( const bool /*newLine*/, const char* const /*name*/, const char* const /*format*/, ...  );

  /**
   * Output normal logging output
   */
  void DebugOutput( const bool newLine, const bool one_shot, const Severity severity, const char* const format, ...  );

  /**
   * Enable/Disable a certain output channel
   */
  LogMgr& EnableOutput(Output output);
  LogMgr& DisableOutput(Output output);
  LogMgr& OpenOutputFile(const char* pFilename);
  LogMgr& BreakOnError(const bool setting);

  /**
   * Enable/disable output of a certain severity on a certain channel
   */
  LogMgr& EnableSeverity(const Output output, const Severity severity);
  LogMgr& DisableSeverity(const Output output, const Severity severity);

  static LogMgr& instance();
  static void close();

  static std::string ToString(char const * const format, ... );

  // macro to create an "anonymous" logger scope
#define GEN_NAME2(prefix, line) prefix##line
#define GEN_NAME(prefix, line) GEN_NAME2(prefix, line)
#define SCOPED_LOG(s) LogMgr::Scope GEN_NAME(log, __LINE__)(s);

  struct Scope {
    Scope(const std::string& str);
    ~Scope();
  };

private:
  LogMgr();
  ~LogMgr();

  void    close_inner();

  void    InitSeverityMap();

  void    OutputInner(const char* str, const Severity severity);

  void    EnterScope(const std::string& str);
  void    LeaveScope();
  void    RecalcScope();

  
  typedef std::deque< std::string > ScopeStack;
  typedef std::map< std::string, std::string > StateLogs;

  Output  output_;
  FILE*   pFile_;

  ScopeStack scopeStack_;
  std::string curScope_;

  StateLogs stateLogs_;
  bool break_on_error_;

  std::set<std::string> one_shot_set_;
  std::map<Output, std::map<Severity, bool> > severity_map_;

  static LogMgr* instance_;
};

/**
*	like printf, but returns a std::string
*/ 
std::string toString(char const * const format, ... );


#endif
