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
    Write("Mesh", render_component.mesh_type);
    Write("Color", render_component.material.diffuse);
    Write("Texture", render_component.material.diffuse_texture);
    Write("Specular", render_component.material.specular);
    Write("Specular Exponent", render_component.material.specular_exp);
    Write("Tint", int8_t(render_component.material.tint));
    EndCompound();
  }
}

template<>
void NBTWriter::Write(string_view name, TransformComponent transform_component)
{
  if (BeginCompound(name))
  {
    Write("Pos", transform_component.pos);
    Write("Scale", transform_component.scale);
    Write("Rotation", transform_component.rotation);
    EndCompound();
  }
}

template<>
void NBTWriter::Write(string_view name, PhysicsComponent physics_component)
{
  if (BeginCompound(name))
  {
    btRigidBody* rigid_body = physics_component.rigid_body;
    float mass = rigid_body->getMass();
    Write("Mass", mass);
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
  if (OpenList(name))
  {
    DirectX::XMFLOAT3 vec;
    vec.x = ReadFloat("");
    vec.y = ReadFloat("");
    vec.z = ReadFloat("");
    CloseList();
    return vec;
  }
  return dx::XMFLOAT3();
}

template<>
DirectX::XMFLOAT4 NBTReader::Read(string_view name)
{
  if (OpenList(name))
  {
    DirectX::XMFLOAT4 vec;
    vec.x = ReadFloat("");
    vec.y = ReadFloat("");
    vec.z = ReadFloat("");
    vec.w = ReadFloat("");
    CloseList();
    return vec;
  }
  return dx::XMFLOAT4();
}
//Fix this
//template<>
//Mesh_Key NBTReader::Read(string_view name)
//{
//  return ReadString(name); //magic_enum::enum_cast<MeshType>(ReadInt(name)).value_or(MeshType::INVALID);
//}

template<>
ComponentKind NBTReader::Read(string_view name)
{
  return magic_enum::enum_cast<ComponentKind>(ReadInt(name)).value_or(ComponentKind::INVALID);
}

}