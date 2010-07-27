#include "stdafx.h"
#include "file_watcher.hpp"
#include "path_utils.hpp"
#include "celsus.hpp"
#include "UnicodeUtils.hpp"
  
enum {
  COMPLETION_KEY_NONE         =   0,
  COMPLETION_KEY_SHUTDOWN     =   1,
  COMPLETION_KEY_IO           =   2
};

FileWatcher *FileWatcher::_instance = nullptr;

FileWatcher::FileWatcher()
  : _watcher_thread(INVALID_HANDLE_VALUE)
  , _dir_handle(INVALID_HANDLE_VALUE)
  , _watcher_completion_port(INVALID_HANDLE_VALUE)
  , _thread_id(0xffffffff)
{
}

FileWatcher& FileWatcher::instance()
{
  if (!_instance)
    _instance = new FileWatcher();
  return *_instance;
}

void FileWatcher::tick()
{
  // process the deferred files
  SCOPED_CS(&_cs_deferred_files);

  if (!_deferred_files.empty()) {
    for (DeferredFiles::iterator i = _deferred_files.begin(), e = _deferred_files.end(); i != e; ++i) {
      const string2& filename = *i;
      // check if the changed file has any registered callbacks
      FileChangedCallbacks::iterator it = _file_changed_callbacks.find(filename);
      if (it != _file_changed_callbacks.end()) {
        for (std::vector<fnFileChanged>::iterator i = it->second.begin(), e = it->second.end(); i != e; ++i) {
          (*i)(filename);
        }
      }
    }

    _deferred_files.clear();
  }
}

bool FileWatcher::init()
{
  InitializeCriticalSection(&_cs_deferred_files);
  _watcher_thread = CreateThread(0, 0, WatcherThread, (void *)this, 0, &_thread_id);
  return _watcher_thread != INVALID_HANDLE_VALUE;
}

bool FileWatcher::close()
{
  _file_changed_callbacks.clear();
  PostQueuedCompletionStatus(_watcher_completion_port, 0, COMPLETION_KEY_SHUTDOWN, 0);
  WaitForSingleObject(_watcher_thread, INFINITE);
  DeleteCriticalSection(&_cs_deferred_files);
  return true;
}

bool FileWatcher::add_file_changed(const string2& filename, const fnFileChanged& fn, const bool initial_load)
{
  auto f = Path::make_canonical(Path::get_full_path_name(filename));
  _file_changed_callbacks[f].push_back(fn);

  // if initial_load is set, we fake a "file changed" event, and call the callback at once
  bool res = true;
  if (initial_load) {
    res = fn(f);
  }

  return res;
}

void FileWatcher::file_changed_internal(const string2& filename)
{
  // queue the file change
  SCOPED_CS(&_cs_deferred_files);
  _deferred_files.insert(filename);

}

DWORD WINAPI FileWatcher::WatcherThread(void* param)
{
  // Directory watcher thread. Uses completion ports to block until it detects a change in the directory tree
  // or until a shutdown event is posted

  FileWatcher *obj = (FileWatcher *)param;

  const int32_t NUM_ENTRIES = 128;
  FILE_NOTIFY_INFORMATION info[NUM_ENTRIES];

  while (true) {

    if ((obj->_dir_handle = CreateFile("./", FILE_LIST_DIRECTORY, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL)) == INVALID_HANDLE_VALUE)
      return 1;

    if ((obj->_watcher_completion_port = CreateIoCompletionPort(obj->_dir_handle, NULL, COMPLETION_KEY_IO, 0)) == INVALID_HANDLE_VALUE)
      return 1;

    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(overlapped));
    if (!ReadDirectoryChangesW(obj->_dir_handle, info, sizeof(info), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &overlapped, NULL))
      return 1;

    DWORD bytes;
    ULONG key = COMPLETION_KEY_NONE;
    OVERLAPPED *overlapped_ptr = NULL;
    bool done = false;
    GetQueuedCompletionStatus(obj->_watcher_completion_port, &bytes, &key, &overlapped_ptr, INFINITE);
    switch (key) {
    case COMPLETION_KEY_NONE: 
      done = true;
      break;
    case COMPLETION_KEY_SHUTDOWN: 
      return 1;

    case COMPLETION_KEY_IO: 
      break;
    }
    CloseHandle(obj->_dir_handle);
    CloseHandle(obj->_watcher_completion_port);

    if (done) {
      break;
    } else {
      info[0].FileName[info[0].FileNameLength/2+0] = 0;
      info[0].FileName[info[0].FileNameLength/2+1] = 0;

      char tmp[MAX_PATH];
      UnicodeToAnsiToBuffer(info[0].FileName, tmp, MAX_PATH);
      const std::string filename(Path::make_canonical(Path::get_full_path_name(tmp)));
      obj->file_changed_internal(filename);
    }
  }
  return 0;
}
