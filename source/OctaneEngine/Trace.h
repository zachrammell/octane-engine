/******************************************************************************/
/*!
  \par        Project Octane
  \file       Trace.h
  \author     Zach Rammell
  \date       2020/10/08
  \brief      A 

  Copyright ?2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <EASTL/fixed_vector.h>

#include <OctaneEngine/EASTLImplementations.h>

#undef ERROR

namespace Octane
{

enum Severity
{
  SERIALIZATION,
  TRACE,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FAILURE,
};

namespace Trace
{

class LogStream
{
public:
  explicit LogStream(std::ostream* log);

  template<typename T>
  friend LogStream& operator<<(LogStream& ls, const T& out);
  friend LogStream& operator<<(LogStream& ls, std::ostream& (*f)(std::ostream&));
  friend LogStream& Log(enum Severity level);
  friend void AddLog(std::ostream* log);
  friend void AddLogColored(std::ostream* log);

private:
  struct StreamEntry
  {
    std::ostream* out_stream_ = nullptr;
    bool is_colored_ = false;
  };
  static constexpr int MAX_STREAMS = 4;
  eastl::fixed_vector<StreamEntry, MAX_STREAMS, false> streams_;

  Severity severity_level_;

  void Attach(std::ostream* log);
  void AttachColored(std::ostream* log);

  void SetSeverity(Severity level);
};

void SetMinSeverity(Severity level);

// Trace::Assert(1 + 1 == 2, "Math works correctly");
void Assert(bool expression, char const* format, ...);
// Trace::Assert(io.valid(), SERIALIZATION, "blah blah file io");
void Assert(bool expression, Severity level, char const* format, ...);
// Trace::Log(DEBUG, "blah blah %c\n", 'c');
int Log(Severity level, char const* format, ...);
// Trace::Log(DEBUG) << "blah blah c++" << std::endl;
LogStream& Log(Severity level);
// Trace::Error("You did something bad! Error code: %d", err);
void Error(char const* format, ...);

void AddLog(std::ostream* log);
void AddLogColored(std::ostream* log);

/* Template implementations */

template<typename T>
LogStream& operator<<(LogStream& ls, const T& out)
{
  if (ls.streams_.empty())
  {
    return ls;
  }
  for (LogStream::StreamEntry const stream_entry : ls.streams_)
  {
    *(stream_entry.out_stream_) << out;
  }
  return ls;
}

/* Template specializations */
template<>
LogStream& operator<<<eastl::string>(LogStream& ls, eastl::string const& out);

}; // namespace Trace

} // namespace Octane
