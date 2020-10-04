#include <OctaneEngine/InputHandler.h>

#include <SDL.h>
#include <imgui_impl_sdl.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/SystemOrder.h>

namespace Octane
{

InputHandler::InputHandler(Engine* engine)
  : ISystem(engine),
    keys_ {new KeyState[SDL_NUM_SCANCODES] {IDLE}},
    prev_keys_ {new KeyState[SDL_NUM_SCANCODES] {IDLE}}
{
}

void InputHandler::Update()
{
  keys_.swap(prev_keys_);
  memset(keys_.get(), 0, SDL_NUM_SCANCODES * sizeof(KeyState));

  for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
  {
    KeyState prev_state = prev_keys_[i];
    if (prev_state == PRESSED || prev_state == HELD)
    {
      keys_[i] = HELD;
    }
  }

  //Event handler
  SDL_Event e;
  //Handle events on queue
  while (SDL_PollEvent(&e) != 0)
  {
    ImGui_ImplSDL2_ProcessEvent(&e);
    
    switch (e.type)
    {
    //User requests quit
    case SDL_QUIT:
    {
      Get<Engine>()->Quit();
    }
    break;
    case SDL_KEYDOWN:
    {
      if (e.key.repeat || ImGui::GetIO().WantCaptureKeyboard)
      {
        break;
      }
      SDL_Scancode const scancode = e.key.keysym.scancode;
      if (prev_keys_[scancode] == IDLE || prev_keys_[scancode] == RELEASED)
      {
        keys_[scancode] = PRESSED;
      }
    }
    break;
    case SDL_KEYUP:
    {
      if (e.key.repeat || ImGui::GetIO().WantCaptureKeyboard)
      {
        break;
      }
      SDL_Scancode const scancode = e.key.keysym.scancode;
      if (prev_keys_[scancode] == PRESSED || prev_keys_[scancode] == HELD)
      {
        keys_[scancode] = RELEASED;
      }
    }
    break;

    default: break;
    }
  }
}

SystemOrder InputHandler::GetOrder()
{
  return SystemOrder::InputHandler;
}

bool InputHandler::KeyPressed(SDL_KeyCode key)
{
  SDL_Scancode const scancode = SDL_GetScancodeFromKey(key);
  return (keys_[scancode] == PRESSED);
}

bool InputHandler::KeyHeld(SDL_KeyCode key)
{
  SDL_Scancode const scancode = SDL_GetScancodeFromKey(key);
  return (keys_[scancode] == HELD);
}

bool InputHandler::KeyReleased(SDL_KeyCode key)
{
  SDL_Scancode const scancode = SDL_GetScancodeFromKey(key);
  return (keys_[scancode] == RELEASED);
}

} // namespace Octane
