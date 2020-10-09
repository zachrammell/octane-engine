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
#pragma once

// Includes
#include <EASTL/string.h>

#include <OctaneEngine/Graphics/Colors.h>
#include <OctaneEngine/EASTLImplementations.h>

namespace Octane
{

namespace FormattedOutput
{

enum ColorANSI : int8_t
{
  Black = 30,
  Crimson,
  Green,
  Olive,
  Navy,
  Purple,
  Aqua,
  Default,
  Gray = 90,
  Red,
  Lime,
  Yellow,
  Blue,
  Pink,
  Cyan,
  White,

  Reset = 0,
};

enum StyleANSI : int8_t
{
  Bold = 1,
  Underlined,
  Inverted,

  Normal = 0,
  NoChange = -1,
};

eastl::string Set(ColorANSI foreground = Reset, ColorANSI background = Reset, StyleANSI decoration = Normal);
eastl::string Set(Color const& foreground, Color const& background = Colors::invalid, StyleANSI decoration = NoChange);
eastl::string Strip(eastl::string&);
eastl::string Strip(eastl::string const&);

bool FormattingEnabled();
void DisableFormatting();
void EnableFormatting();

} // namespace FormattedOutput

} // namespace Octane
