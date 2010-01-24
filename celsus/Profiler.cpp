#include "stdafx.h"
#include "Profiler.hpp"

Profiler* Profiler::instance_ = NULL;

Profiler& Profiler::instance()
{
  if (instance_ == NULL) {
    instance_ = new Profiler();
    atexit(close);
  }
  return *instance_;
}

void Profiler::enter_scope(const char* name)
{
  LARGE_INTEGER cur_time;
  QueryPerformanceCounter(&cur_time);
  Scope* new_scope = new Scope(name, cur_time);

  if (parent_stack_.empty()) {
    top_level_.push_back(new_scope);
  } else {
    parent_stack_.back()->children_.push_back(new_scope);
  }

  parent_stack_.push_back(new_scope);
}

void Profiler::leave_scope()
{
  if (parent_stack_.empty()) {
    return;
  }
  LARGE_INTEGER cur_time;
  QueryPerformanceCounter(&cur_time);
  parent_stack_.back()->leave_ = cur_time;
  parent_stack_.pop_back();
}

void Profiler::print()
{
  // Leave the outstanding scopes
  const uint32_t outstanding_stacks = parent_stack_.size();
  for (uint32_t i = 0; i < outstanding_stacks; ++i) {
    leave_scope();
  }

  for (TopLevel::const_iterator it = top_level_.begin(); it != top_level_.end(); ++it) {
    print_inner(*it, "");
  }
}

void Profiler::print_inner(const Scope* cur, const std::string& indent)
{
  LOG_WARNING_LN("%s: %f", 
    (indent + cur->name_).c_str(), 
    (cur->leave_.QuadPart - cur->enter_.QuadPart) / (double)frequency_.QuadPart);
  const std::string new_indent(indent + "  ");
  for (Scope::Children::const_iterator it = cur->children_.begin(); it != cur->children_.end(); ++it) {
    print_inner(*it, new_indent);
  }
}

Profiler::Profiler()
{
  QueryPerformanceFrequency(&frequency_);
}

Profiler::~Profiler()
{
  container_delete(top_level_);
}

void Profiler::close()
{
  SAFE_DELETE(instance_);
}
