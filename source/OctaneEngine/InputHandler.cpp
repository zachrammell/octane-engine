#include <OctaneEngine/InputHandler.h>

#include <SDL.h>
#include <imgui_impl_sdl.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Helper.h>
#include <OctaneEngine/SDLEventHandler.h>
#include <OctaneEngine/WindowManager.h>

namespace Octane
{

InputHandler::InputHandler(Engine* engine)
  : ISystem(engine),
    keys_ {new KeyState[SDL_NUM_SCANCODES] {IDLE}},
    prev_keys_ {new KeyState[SDL_NUM_SCANCODES] {IDLE}},
    mouse_buttons_ {new KeyState[to_integral(MouseButton::COUNT)] {IDLE}},
    prev_mouse_buttons_ {new KeyState[to_integral(MouseButton::COUNT)] {IDLE}}
{
}

void InputHandler::Update()
{
  ProcessAllSDLEvents();
  HandleGlobalHotkeys();
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

bool InputHandler::KeyPressedOrHeld(SDL_KeyCode key)
{
  SDL_Scancode const scancode = SDL_GetScancodeFromKey(key);
  return keys_[scancode] == PRESSED || keys_[scancode] == HELD;
}

bool InputHandler::KeyReleased(SDL_KeyCode key)
{
  SDL_Scancode const scancode = SDL_GetScancodeFromKey(key);
  return (keys_[scancode] == RELEASED);
}

bool InputHandler::MouseButtonPressed(MouseButton button)
{
  return mouse_buttons_[to_integral(button)] == PRESSED;
}

bool InputHandler::MouseButtonHeld(MouseButton button)
{
  return mouse_buttons_[to_integral(button)] == HELD;
}

bool InputHandler::MouseButtonPressedOrHeld(MouseButton button)
{
  return mouse_buttons_[to_integral(button)] == HELD || mouse_buttons_[to_integral(button)] == PRESSED;
}

bool InputHandler::MouseButtonReleased(MouseButton button)
{
  return mouse_buttons_[to_integral(button)] == RELEASED;
}

DirectX::XMINT2 InputHandler::GetMouseMovement()
{
  return mouse_movement_;
}
void InputHandler::ProcessAllSDLEvents()
{
  keys_.swap(prev_keys_);
  memset(keys_.get(), 0, SDL_NUM_SCANCODES * sizeof(KeyState));

  mouse_buttons_.swap(prev_mouse_buttons_);
  memset(mouse_buttons_.get(), 0, to_integral(MouseButton::COUNT) * sizeof(KeyState));

  for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
  {
    KeyState prev_state = prev_keys_[i];
    if (prev_state == PRESSED || prev_state == HELD)
    {
      keys_[i] = HELD;
    }
  }

  for (int i = 0; i < to_integral(MouseButton::COUNT); ++i)
  {
    KeyState prev_state = prev_mouse_buttons_[i];
    if (prev_state == PRESSED || prev_state == HELD)
    {
      mouse_buttons_[i] = HELD;
    }
  }

  mouse_movement_ = {0, 0};

  //Event handler
  SDL_Event e;
  //Handle events on queue
  while (SDL_PollEvent(&e) != 0)
  {
    ImGui_ImplSDL2_ProcessEvent(&e);

    switch (e.type)
    {
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
    case SDL_MOUSEMOTION:
    {
      if (ImGui::GetIO().WantCaptureMouse)
      {
        break;
      }
      mouse_movement_ = {e.motion.xrel, e.motion.yrel};
      mouse_position_ = {e.motion.x, e.motion.y};
    }
    break;
    case SDL_MOUSEBUTTONDOWN:
    {
      if (ImGui::GetIO().WantCaptureMouse)
      {
        break;
      }
      if (prev_mouse_buttons_[e.button.button] == IDLE || prev_mouse_buttons_[e.button.button] == RELEASED)
      {
        mouse_buttons_[e.button.button] = PRESSED;
      }
    }
    break;
    case SDL_MOUSEBUTTONUP:
    {
      if (ImGui::GetIO().WantCaptureMouse)
      {
        break;
      }
      if (prev_mouse_buttons_[e.button.button] == PRESSED || prev_mouse_buttons_[e.button.button] == HELD)
      {
        mouse_buttons_[e.button.button] = RELEASED;
      }
    }
    break;

    default:
    {
      // all non-input events go here
      HandleSDLEvent(e);
      break;
    }
    }
  }
}
void InputHandler::HandleGlobalHotkeys()
{
  // temporary bind: f7 to toggle fullscreen
  if (KeyPressed(SDLK_F7))
  {
    bool fullscreen = Get<WindowManager>()->IsFullscreen();
    Get<WindowManager>()->SetFullscreen(!fullscreen);
  }
}
} // namespace Octane
