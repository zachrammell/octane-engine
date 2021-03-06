#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <OctaneEngine/Engine.h>

#include <OctaneEngine/FormattedOutput.h>
#include <OctaneEngine/Registry.h>
#include <OctaneEngine/Trace.h>

#include <OctaneEngine/Audio.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/MeshSys.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/ImGuiSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/PlayerMovementControllerSys.h>
#include <OctaneEngine/SceneSys.h>
#include <OctaneEngine/WindowManager.h>
#include <OctaneEngine/Graphics/TextureSys.h>

namespace fs = std::filesystem;
using namespace Octane::FormattedOutput;

namespace
{

void AddSystems(Octane::Engine& engine)
{
  engine.AddSystem(new Octane::FramerateController {&engine});
  engine.AddSystem(new Octane::InputHandler {&engine});
  engine.AddSystem(new Octane::WindowManager {&engine, "Project Octane", 1280, 720}); //Can do 1280x1024
  engine.AddSystem(new Octane::CameraSys {&engine});
  engine.AddSystem(new Octane::PhysicsSys {&engine});
  engine.AddSystem(new Octane::EntitySys {&engine});
  engine.AddSystem(new Octane::ComponentSys {&engine});
  engine.AddSystem(new Octane::SceneSys {&engine});
  engine.AddSystem(new Octane::RenderSys {&engine});
  engine.AddSystem(new Octane::BehaviorSys {&engine});
  engine.AddSystem(new Octane::PlayerMovementControllerSys {&engine});
  engine.AddSystem(new Octane::TextureSys {&engine});
  // depends on WindowSys and RenderSys
  engine.AddSystem(new Octane::ImGuiSys {&engine});
  engine.AddSystem(new Octane::MeshSys {&engine}); //depends on TextureSys
  engine.AddSystem(new Octane::Audio {&engine});
}

void RunGame()
{
  // create sandbox folder for test files
  if (!fs::exists("sandbox"))
  {
    fs::create_directory("sandbox");
  }
  std::ofstream logfile("sandbox/latest.txt");

  {
    Octane::Trace::AddLog(&logfile);
    if (
      IsDebuggerPresent()
      || Octane::Registry::GetDword(HKEY_CURRENT_USER, "Console", "VirtualTerminalLevel").value_or(0) == 0x1)
    {
      Octane::Trace::AddLogColored(&std::clog);
    }
    else
    {
      Octane::Trace::AddLog(&std::clog);
    }

    Octane::Trace::Log(Octane::INFO) << "[== Project Octane ==]\n";
  }

  Octane::CreateEngine();
  Octane::Engine* engine = Octane::GetEngine();
  AddSystems(*engine);

  while (!engine->ShouldQuit())
  {
    engine->Update();
  }
  Octane::DeleteEngine();
}

} // namespace

int main(int argc, char* argv[]) noexcept
{
  // must be the first thing in main, for SDL2 initialization
  SDL_SetMainReady();

  RunGame();

  _CrtDumpMemoryLeaks();
  return 0;
}
