#include <OctaneEngine/SDLEventHandler.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Graphics/RenderSys.h>

namespace Octane
{
void HandleSDLEvent(SDL_Event const& event)
{
  // input events are handled in inputhandler.h
  switch (event.type)
  {
  case SDL_WINDOWEVENT_RESIZED:
  {
    // pause the framerate controller during the slow process of rebuilding framebuffers
    auto* frc = GetEngine()->GetSystem<FramerateController>();
    bool was_paused = frc->IsPaused();

    if (was_paused)
    {
      frc->Pause();
    }
    GetEngine()->GetSystem<RenderSys>()->OnResize();

    if (was_paused)
    {
      frc->Unpause();
    }
  }
  case SDL_QUIT:
  {
    //User requests quit
    GetEngine()->Quit();
    break;
  }
  default: break;
  }
}
} // namespace Octane