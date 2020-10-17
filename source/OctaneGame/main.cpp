#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <filesystem>
#include <iostream>
#include <fstream>

#define SDL_MAIN_HANDLED

#include <EASTL/vector.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/OBJParser.h>
#include <OctaneEngine/Graphics/GraphicsDeviceDX11.h>

#include <OctaneEngine/ImGuiSys.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FormattedOutput.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/NBTWriter.h>
#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/Physics/RigidBody.h>
#include <OctaneEngine/SceneSys.h>
#include <OctaneEngine/Trace.h>
#include <OctaneEngine/WindowManager.h>
#include <OctaneEngine/Graphics/RenderSys.h>

namespace fs = std::filesystem;
using namespace Octane::FormattedOutput;

int main(int argc, char* argv[]) noexcept
{
  // must be the first thing in main, for SDL2 initialization
  SDL_SetMainReady();

  // create sandbox folder for test files
  if (!fs::exists("sandbox"))
  {
    fs::create_directory("sandbox");
  }

  std::ofstream logfile("sandbox/latest.txt");

  Octane::Trace::AddLog(&logfile);
  Octane::Trace::AddLog(&std::clog);
  Octane::Trace::Log(Octane::INFO) << "[== Project Octane ==]\n";

  Octane::Engine engine;
  engine.AddSystem(new Octane::FramerateController {&engine});
  engine.AddSystem(new Octane::InputHandler {&engine});
  engine.AddSystem(new Octane::WindowManager {&engine, "Project Octane", 1280, 720});
  engine.AddSystem(new Octane::CameraSys {&engine});
  engine.AddSystem(new Octane::PhysicsSys {&engine});
  engine.AddSystem(new Octane::EntitySys {&engine});
  engine.AddSystem(new Octane::ComponentSys {&engine});
  engine.AddSystem(new Octane::SceneSys {&engine});
  engine.AddSystem(new Octane::RenderSys {&engine});
  // depends on WindowSys and RenderSys
  engine.AddSystem(new Octane::ImGuiSys {&engine});

  // NBT writing demo
  {
    Octane::NBTWriter nbt_writer("sandbox/test_list.nbt");
    nbt_writer.WriteInt("hi", 300);
    if (nbt_writer.BeginCompound("stuff"))
    {
      if (nbt_writer.BeginList("players"))
      {
        if (nbt_writer.BeginCompound(""))
        {
          nbt_writer.WriteByte("lives", 3);
          nbt_writer.WriteFloat("health", 56.7f);
          if (nbt_writer.BeginList("inventory"))
          {
            if (nbt_writer.BeginCompound(""))
            {
              nbt_writer.WriteInt("id", 276);
              nbt_writer.WriteByte("count", 1);
              nbt_writer.EndCompound();
            }
            if (nbt_writer.BeginCompound(""))
            {
              nbt_writer.WriteInt("id", 46);
              nbt_writer.WriteByte("count", 64);
              nbt_writer.EndCompound();
            }
            nbt_writer.EndList(); // inventory
          }
          nbt_writer.EndCompound();
        }
        nbt_writer.EndList(); // players
      }
      if (nbt_writer.BeginList("vehicles"))
      {
        nbt_writer.WriteString("", "car");
        nbt_writer.WriteString("", "truck");
        nbt_writer.WriteString("", "subaru wrx");
        nbt_writer.WriteString("", "bike");
        nbt_writer.EndList(); // vehicles
      }
      nbt_writer.EndCompound(); // stuff
    }
  }

  while (!engine.ShouldQuit())
  {
    engine.Update(); 
  }

  _CrtDumpMemoryLeaks();
  return 0;
}
