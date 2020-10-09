/******************************************************************************/
/*!
  \par        Project Octane
  \file       FormattedOutput.cpp
  \author     Zach Rammell
  \date       2020/10/08
  \brief      <WHAT DOES IT DO>

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#include <OctaneEngine/FormattedOutput.h>

/* Needed for impl. of <<, without this it just infinitely recurses.
This is because the only definition it can find is its own, and because
char* as returned by c_str() is implicitly convertible to eastl::string, it will just call itself forever. */
#include <iostream> 

namespace eastl
{

std::ostream& operator<<(std::ostream& lhs, eastl::string const& rhs)
{
  // it just works
  return lhs << (rhs.c_str());
}

}


namespace
{

constexpr int BG_offset = 10;
bool formatting_enabled = true;

} // namespace

namespace Octane
{

namespace FormattedOutput
{
eastl::string Strip(eastl::string& text)
{
  if (!FormattingEnabled())
  {
    return text;
  }
  return Strip(const_cast<eastl::string const&>(text));
}

eastl::string Strip(eastl::string const& text)
{
  if (!FormattingEnabled())
  {
    return text;
  }
  eastl::string ret {};
  ret.resize(text.length());
  bool in_format_code = false;
  int bytes_written = 0;
  for (char c : text)
  {
    if (in_format_code)
    {
      if (c == 'm')
      {
        in_format_code = false;
      }
      continue;
    }
    if (c == '\033')
    {
      in_format_code = true;
      continue;
    }
    ret[bytes_written] = c;
    ++bytes_written;
  }
  return ret;
}

bool FormattingEnabled()
{
  return formatting_enabled;
}

void DisableFormatting()
{
  formatting_enabled = false;
}

void EnableFormatting()
{
  formatting_enabled = true;
}

eastl::string Set(ColorANSI foreground, ColorANSI background, StyleANSI decoration)
{
  if (!FormattingEnabled())
  {
    return "";
  }

  /* An escape sequence of this type can be at most 15 bytes long. */
  eastl::string escape_sequence {eastl::string::CtorDoNotInitialize {}, 16};

  escape_sequence.append("\033[");

  int print_count = (foreground != Reset) + (background != Reset) + (decoration != Normal);

  if (print_count < 3)
  {
    escape_sequence.append("0;");
  }
  
  if (foreground != Reset)
  {
    escape_sequence.append_sprintf("%d", foreground);
    if (--print_count != 0)
    {
      escape_sequence.append(";");
    }
  }

  if (background != Reset)
  {
    escape_sequence.append_sprintf("%d", background + BG_offset);
    if (--print_count != 0)
    {
      escape_sequence.append(";");
    }
  }

  if (decoration != Normal)
  {
    escape_sequence.append_sprintf("%d", decoration);
    if (--print_count != 0)
    {
      escape_sequence.append(";");
    }
  }

  escape_sequence.append("m");
  return escape_sequence;
}

eastl::string Set(Color const& foreground, Color const& background, StyleANSI decoration)
{
  if (!FormattingEnabled())
  {
    return "";
  }

  int print_count = 3 + (3 * (background != Colors::invalid)) + (decoration != NoChange);

  /* An escape sequence of this type can be at most 39 bytes long. */
  eastl::string escape_sequence {eastl::string::CtorDoNotInitialize {}, 64};

  escape_sequence.append("\033[");

  if (decoration != NoChange)
  {
    escape_sequence.append_sprintf("%d", decoration);
    if (--print_count)
    {
      escape_sequence.append(";");
    }
  }

  /* Escape code for 24-bit RGB (true color) output in the foreground. */
  escape_sequence.append("38;2;");

  escape_sequence.append_sprintf("%d;", static_cast<int>(foreground.r * 255));
  escape_sequence.append_sprintf("%d;", static_cast<int>(foreground.g * 255));
  escape_sequence.append_sprintf("%d", static_cast<int>(foreground.b * 255));

  if (print_count -= 3)
  {
    escape_sequence.append(";");
  }

  if (background != Colors::invalid)
  {
    /* Escape code for 24-bit RGB (true color) output in the background. */
    escape_sequence.append("48;2;");

    escape_sequence.append_sprintf("%d;", static_cast<int>(background.r * 255));
    escape_sequence.append_sprintf("%d;", static_cast<int>(background.g * 255));
    escape_sequence.append_sprintf("%d",  static_cast<int>(background.b * 255));

    if (print_count -= 3)
    {
      escape_sequence.append(";");
    }
  }

  escape_sequence.append("m");

  return escape_sequence;
}

} // namespace FormattedOutput

} // namespace Octane
