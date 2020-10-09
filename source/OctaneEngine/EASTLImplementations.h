/******************************************************************************/
/*!
  \par        Project Octane
  \file       EASTLImplementations.h
  \author     Zach Rammell
  \date       2020/10/08
  \brief      The EA STL has a variety of things that need to be implemented by
              the user of the library. This is where those things go.
              Additionally, our own extensions to the library (like printable
              strings... -_-) also go here.

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once
#include <EASTL/string.h>
#include <iosfwd>

/*
 * "Why is this in the eastl namespace?? That's dumb!"
 * Dear reader, let me take you on a C++ journey.
 *
 * To begin with, EASTL does not implement ostream output operators for its strings.
 * This means they need to be implemented as a free function.
 * During template instantiation, a function template depending on a template type is
 * only found during phase II look-up. Phase II look-up doesn't consider names visible
 * at the point of use but only considers names found based on argument dependent look-up.
 * Since the only associated namespaces for std::ostream and eastl::string are namespaces
 * std and eastl, it doesn't look for the output operators defined in the global namespace.
 *
 * Moving the declarations around doesn't help: phase II name look-up doesn't really depend on
 * the order of the declaration except that the declarations have to precede the point of
 * instantiation. The only proper fix is to define the operators in a namespace being sought by
 * phase II look-up which pretty much means that the operator has to be defined in namespace eastl.
 * Because it's illegal to define anything in namespace std except template specializations
 * on user-defined types.
 *
 * So here we are. Man, I can't wait for Jai.
 */
namespace eastl
{
// output operator for EASTL strings
std::ostream& operator<<(std::ostream& lhs, eastl::string const& rhs);
} // namespace eastl
