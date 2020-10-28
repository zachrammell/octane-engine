#pragma once

#define VC_EXTRALEAN
#include <Windows.h>
#include <EASTL/string_view.h>
#include <EASTL/optional.h>

namespace Octane::Registry
{

eastl::optional<DWORD> GetDword(HKEY hKey, eastl::string_view subKey, eastl::string_view value);

}
