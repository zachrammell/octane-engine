/******************************************************************************/
/*!
  \par        Project Octane
  \file       EASTLImplementations.cpp
  \author     Zach Rammell
  \date       2020/10/08
  \brief      The EA STL has a variety of things that need to be implemented by
              the user of the library. This is where those things go.
              Additionally, our own extensions to the library (like printable
              strings) also go here.

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#include <OctaneEngine/EASTLImplementations.h>
#include <cstdint>
#include <cstdio>
#include <cwchar>

// EASTL expects user-defined new[] operators that it will use for memory allocation.
// TODO: make these return already-allocated memory from our own memory allocator.
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
  return new uint8_t[size];
}

void* operator new[](
  size_t size,
  size_t alignment,
  size_t alignmentOffset,
  const char* pName,
  int flags,
  unsigned debugFlags,
  const char* file,
  int line)
{
  // pass-through to standard memory manager for now, will later be a custom one
  return new uint8_t[size];
}

// yoinked from EASTL documentation
namespace EA::StdC
{

int __cdecl Vsnprintf(char* __restrict p, size_t n, const char* __restrict pFormat, va_list arguments)
{
#ifdef _MSC_VER
  return vsnprintf(p, n, pFormat, arguments);
#else
  return vsnprintf(p, n, pFormat, arguments);
#endif
}

int __cdecl Vsnprintf(wchar_t* __restrict p, size_t n, const wchar_t* __restrict pFormat, va_list arguments)
{
#ifdef _MSC_VER
  return vswprintf(p, n, pFormat, arguments);
#else
  return vsnwprintf(p, n, pFormat, arguments); // Won't work on Unix because its libraries implement wchar_t as int32_t.
#endif
}

} // namespace EA::StdC
