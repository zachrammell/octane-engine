#include <OctaneEngine/Registry.h>

namespace Octane::Registry
{

eastl::optional<DWORD> GetDword(HKEY hKey, eastl::string_view subKey, eastl::string_view value)
{
  DWORD data {};
  DWORD dataSize = sizeof(data);
  LONG const retCode = RegGetValue(hKey, subKey.data(), value.data(), RRF_RT_REG_DWORD, nullptr, &data, &dataSize);
  if (retCode != ERROR_SUCCESS)
  {
    return eastl::nullopt;
  }
  return eastl::make_optional(data);
}

}
