#include <OctaneEngine/NBTSpecializations.h>

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Helper.h>
#include <magic_enum.hpp>

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

template<>
void NBTWriter::Write(string_view name, RenderComponent render_component)
{
  if (BeginCompound(name))
  {
    Write("ComponentType", to_integral(ComponentKind::Render));
    Write("Mesh", to_integral(render_component.mesh_type));
    Write("Color", render_component.color);
    EndCompound();
  }
}

template<>
void NBTWriter::Write(string_view name, TransformComponent transform_component)
{
  if (BeginCompound(name))
  {
    Write("ComponentType", to_integral(ComponentKind::Transform));
    Write("Pos", transform_component.pos);
    Write("Scale", transform_component.scale);
    Write("Rotation", transform_component.rotation);
    EndCompound();
  }
}

template<>
Color NBTReader::Read(string_view name)
{
  Color color;
  color.vec = Read<DirectX::XMFLOAT3>(name);
  return color;
}

template<>
DirectX::XMFLOAT3 NBTReader::Read(string_view name)
{
  if (EnterList(name))
  {
    DirectX::XMFLOAT3 vec;
    vec.x = ReadFloat("");
    vec.y = ReadFloat("");
    vec.z = ReadFloat("");
    ExitList();
    return vec;
  }
}

template<>
DirectX::XMFLOAT4 NBTReader::Read(string_view name)
{
  if (EnterList(name))
  {
    DirectX::XMFLOAT4 vec;
    vec.x = ReadFloat("");
    vec.y = ReadFloat("");
    vec.z = ReadFloat("");
    vec.w = ReadFloat("");
    ExitList();
    return vec;
  }
}

template<>
MeshType NBTReader::Read(string_view name)
{
  return magic_enum::enum_cast<MeshType>(ReadInt(name)).value_or(MeshType::INVALID);
}

}