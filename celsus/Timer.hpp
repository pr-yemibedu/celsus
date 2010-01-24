#ifndef TIMER_HPP
#define TIMER_HPP
#include <windows.h>

class Timer
{
public:
  Timer()
  {
    QueryPerformanceFrequency(&frequency_);
  }

  void start()
  {
    QueryPerformanceCounter(&start_);
  }

  void stop()
  {
    QueryPerformanceCounter(&stop_);
  }

  double duration() const
  {
    return (stop_.QuadPart - start_.QuadPart) / (double)frequency_.QuadPart;
  }

private:
  LARGE_INTEGER start_;
  LARGE_INTEGER stop_;
  LARGE_INTEGER frequency_;
};

#endif