#pragma once
#include <functional>
#include <set>
#include "string_utils.hpp"

class FileWatcher
{
public:
  typedef std::function<bool (const string2&)> fnFileChanged;

  static FileWatcher& instance();
  bool add_file_changed(const string2& filename, const fnFileChanged& fn, const bool initial_load);
  bool init();
  void tick();
  bool close();
private:
  FileWatcher();
  static FileWatcher *_instance;

  void file_changed_internal(const string2& filename);
  static DWORD WINAPI WatcherThread(void* param);

  typedef std::map< string2, std::vector<fnFileChanged> > FileChangedCallbacks;
  typedef std::set< string2 > DeferredFiles;

  CRITICAL_SECTION _cs_deferred_files;
  FileChangedCallbacks _file_changed_callbacks;
  DeferredFiles _deferred_files;

  DWORD _thread_id;
  HANDLE _watcher_thread;
  HANDLE _dir_handle;
  HANDLE _watcher_completion_port;
};
