#include <OctaneEngine/FramerateController.h>

#include <SDL.h>

#include <OctaneEngine/SystemOrder.h>

namespace Octane
{

void FramerateController::LevelStart()
{
  /* Initialize current_time_ on level start because if it is done in construction,
   * an unspecified amount of time will elapse between then and the first frame.
   * This keeps the potential delay very small. */
  current_time_ = SDL_GetPerformanceCounter();
}

void FramerateController::Update()
{
  {
    uint64_t const previous_time = current_time_;
    current_time_ = SDL_GetPerformanceCounter();
    delta_time_ = static_cast<float>(current_time_ - previous_time) / static_cast<float>(SDL_GetPerformanceFrequency());

    /* Prevents dt from reaching unreasonable values and causing things to freak out
     * 1 frame after dragging the window or after step-through debugging, for instance. */
    if (delta_time_ > .25f)
    {
      // 1/60 of a second is a reasonable time for one frame. ;)
      delta_time_ = (1.0f / 60.0f);
    }
  }
}

SystemOrder FramerateController::GetOrder()
{
  return SystemOrder::FramerateController;
}

float FramerateController::GetDeltaTime()
{
  return delta_time_;
}

} // namespace Octane
