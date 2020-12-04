#pragma once

#include <SDL.h>

namespace Octane
{
// this is called by the event loop code in inputhandler.cpp for all non-input related events
void HandleSDLEvent(SDL_Event const& e);
} // namespace Octane