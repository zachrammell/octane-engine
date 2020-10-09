/******************************************************************************/
/*!
  \par        Project Octane
  \file       <THIS FILENAME>
  \author     <WHO WROTE THIS>
  \date       YYYY/MM/DD
  \brief      <WHAT DOES IT DO>

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#include <OctaneEngine/Trace.h>

#include <cstdarg>
#include <cstdio>
#include <iostream> // iostream, clog

#include <OctaneEngine/FormattedOutput.h>

using namespace Octane::FormattedOutput;

namespace Octane::Trace
{

namespace
{

LogStream log_stream {nullptr};
LogStream log_null {nullptr};

} // namespace

ColorANSI const severity_colors[] = {
  Blue,    // SERIALIZATION
  Aqua,    // TRACE
  Green,   // DEBUG
  Lime,    // INFO
  Yellow,  // WARNING
  Red,     // ERROR
  Crimson, // FAILURE
};

char const* const severity_names[] = {"SERIALIZATION", "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FAILURE"};

Severity min_severity = SERIALIZATION;

LogStream::LogStream(std::ostream* log)
{
  if (log)
  {
    Attach(log);
  }
}

void LogStream::Attach(std::ostream* log)
{
  streams_.push_back({log, false});
}

void LogStream::AttachColored(std::ostream* log)
{
  streams_.push_back({log, true});
}

LogStream& operator<<(LogStream& ls, std::ostream& (*f)(std::ostream&))
{
  if (ls.streams_.empty())
  {
    return ls;
  }
  for (LogStream::StreamEntry const stream_entry : ls.streams_)
  {
    f(*(stream_entry.out_stream_));
  }
  return ls;
}

void SetMinSeverity(Severity level)
{
  min_severity = level;
}

static void Assert_Internal(bool expression, Severity level, char const* format, std::va_list args)
{
  eastl::string formatted;
  if (expression)
  {
    Log(level) << "Asserted that " << formatted.sprintf_va_list(format, args) << "\n";
  }
  else
  {
    Log(FAILURE) << "Failed to assert that " << formatted.sprintf_va_list(format, args) << "\n";
    assert(expression);
  }
}

void Assert(bool expression, char const* format, ...)
{
  std::va_list args;
  va_start(args, format);
  Assert_Internal(expression, DEBUG, format, args);
  va_end(args);
}

void Assert(bool expression, Severity level, char const* format, ...)
{
  std::va_list args;
  va_start(args, format);
  Assert_Internal(expression, level, format, args);
  va_end(args);
}

int Log(Severity level, char const* format, ...)
{
  std::va_list args;
  eastl::string formatted;
  va_start(args, format);
  Log(level) << formatted.sprintf_va_list(format, args);
  va_end(args);
  return formatted.length();
}

LogStream& Log(Severity level)
{
  if (level >= min_severity)
  {
    for (LogStream::StreamEntry const stream_entry : log_stream.streams_)
    {
      *(stream_entry.out_stream_) << "[";
      if (stream_entry.is_colored_)
      {
        *(stream_entry.out_stream_) << Set(severity_colors[level]) << severity_names[level] << Set();
      }
      else
      {
        *(stream_entry.out_stream_) << severity_names[level];
      }
      *(stream_entry.out_stream_) << "]: ";
    }
    return log_stream;
  }
  return log_null;
}

void AddLog(std::ostream* log)
{
  log_stream.Attach(log);
}

void AddLogColored(std::ostream* log)
{
  log_stream.AttachColored(log);
}

template<>
LogStream& operator<<<eastl::string>(LogStream& ls, eastl::string const& out)
{
  if (ls.streams_.empty())
  {
    return ls;
  }
  for (LogStream::StreamEntry const stream_entry : ls.streams_)
  {
    if (stream_entry.is_colored_)
    {
      *(stream_entry.out_stream_) << out;
    }
    else
    {
      *(stream_entry.out_stream_) << Strip(out);
    }
  }
  return ls;
}

} // namespace Octane::Trace
