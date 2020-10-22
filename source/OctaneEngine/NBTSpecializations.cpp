#include <OctaneEngine/NBTSpecializations.h>

namespace Octane
{

template<>
void NBTWriter::Write(string_view name, Color color)
{
  Write(name, color.vec);
}
template<>
void NBTWriter::Write(string_view name, DirectX::XMFLOAT3 vec)
{
  if (BeginList(name))
  {
    WriteFloat("", vec.x);
    WriteFloat("", vec.y);
    WriteFloat("", vec.z);
    EndList();
  }
}
template<>
void NBTWriter::Write(string_view name, DirectX::XMFLOAT4 vec)
{
  if (BeginList(name))
  {
    WriteFloat("", vec.x);
    WriteFloat("", vec.y);
    WriteFloat("", vec.z);
    WriteFloat("", vec.w);
    EndList();
  }
}

}