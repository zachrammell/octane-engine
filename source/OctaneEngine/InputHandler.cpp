#include <OctaneEngine/InputHandler.h>

#include <SDL.h>
#include <imgui_impl_sdl.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/SystemOrder.h>

namespace Octane
{

InputHandler::InputHandler(Engine* engine) : engine_ {engine} {}

void InputHandler::Update()
{
  //Event handler
  SDL_Event e;
  //Handle events on queue
  while (SDL_PollEvent(&e) != 0)
  {
    ImGui_ImplSDL2_ProcessEvent(&e);
    //User requests quit
    if (e.type == SDL_QUIT)
    {
      engine_->Quit();
    }
  }
}

SystemOrder InputHandler::GetOrder()
{
  return SystemOrder::InputHandler;
}

} // namespace Octane
