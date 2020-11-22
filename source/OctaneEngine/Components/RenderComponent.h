#pragma once

#include <OctaneEngine/Graphics/Colors.h>

#include <EASTL/string_view.h>
namespace Octane
{

//enum class MeshType : int32_t
//{
//  INVALID = -1,
//  Cube,
//  Sphere,
//  Cube_Rounded,
//  Bear,
//  Duck,
//  Bunny,
//  Crossbow,
//  Slingshot,
//  Shuriken,
//  PaperPlane,
//  TestFBX,
//  PaperStack,
//  Quad,
//  Sword,
//  Sniper1,
//  Semiauto1,
//
//  COUNT
//};

enum class RenderType : int32_t
{
  Filled,
  Wireframe,

  COUNT
};

enum class ShaderType : int32_t
{
  Phong,
  UI,

  COUNT
};

typedef eastl::string_view Mesh_Key; //MeshSys mesh key type


struct RenderComponent
{
  Color color {};
  Mesh_Key mesh_type;
  //MeshType mesh_type = MeshType::INVALID;
  RenderType render_type = RenderType::Filled;
  ShaderType shader_type = ShaderType::Phong;
};

} // namespace Octane
