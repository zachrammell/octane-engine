/******************************************************************************/
/*!
  \par        Project Octane
  \file       ImGuiSys.h
  \author     Zach Rammell
  \date       2020/10/15
  \brief      Initializes and updates the ImGui library

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#include <OctaneEngine/ImGuiSys.h>
#include <OctaneEngine/SystemOrder.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/WindowManager.h>

namespace Octane
{

ImGuiSys::ImGuiSys(Engine* parent_engine) : ISystem(parent_engine)
{
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
#if 0
  // Multi-viewport is the setting that allows imgui to draw its own windows outside the main game window
  // disabled for now, as on some setups the menu options draw off-screen and become unusable
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
#endif

  //io.ConfigViewportsNoTaskBarIcon = true;
  io.IniFilename = "editor_config.ini";

  //ImGui::StyleColorsDark();
  StyleVisualStudio();
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplSDL2_InitForD3D(parent_engine->GetSystem<WindowManager>()->GetHandle());
  RenderSys* render_sys = parent_engine->GetSystem<RenderSys>();
  ImGui_ImplDX11_Init(
    render_sys->GetGraphicsDeviceDX11().GetD3D11Device(),
    render_sys->GetGraphicsDeviceDX11().GetD3D11Context());
}

void ImGuiSys::Update()
{
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplSDL2_NewFrame(Get<WindowManager>()->GetHandle());
  ImGui::NewFrame();
}

SystemOrder ImGuiSys::GetOrder()
{
  return SystemOrder::ImGuiSys;
}

void ImGuiSys::StyleVisualStudio()
{
  constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b) {
    return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
  };

  auto& style = ImGui::GetStyle();
  ImVec4* colors = style.Colors;

  const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
  const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
  const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

  const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
  const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
  const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

  const ImVec4 textColor = ColorFromBytes(255, 255, 255);
  const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
  const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

  colors[ImGuiCol_Text] = textColor;
  colors[ImGuiCol_TextDisabled] = textDisabledColor;
  colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
  colors[ImGuiCol_WindowBg] = bgColor;
  colors[ImGuiCol_ChildBg] = bgColor;
  colors[ImGuiCol_PopupBg] = bgColor;
  colors[ImGuiCol_Border] = borderColor;
  colors[ImGuiCol_BorderShadow] = borderColor;
  colors[ImGuiCol_FrameBg] = panelColor;
  colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
  colors[ImGuiCol_FrameBgActive] = panelActiveColor;
  colors[ImGuiCol_TitleBg] = bgColor;
  colors[ImGuiCol_TitleBgActive] = bgColor;
  colors[ImGuiCol_TitleBgCollapsed] = bgColor;
  colors[ImGuiCol_MenuBarBg] = panelColor;
  colors[ImGuiCol_ScrollbarBg] = panelColor;
  colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
  colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
  colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
  colors[ImGuiCol_CheckMark] = panelActiveColor;
  colors[ImGuiCol_SliderGrab] = panelHoverColor;
  colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
  colors[ImGuiCol_Button] = panelColor;
  colors[ImGuiCol_ButtonHovered] = panelHoverColor;
  colors[ImGuiCol_ButtonActive] = panelHoverColor;
  colors[ImGuiCol_Header] = panelColor;
  colors[ImGuiCol_HeaderHovered] = panelHoverColor;
  colors[ImGuiCol_HeaderActive] = panelActiveColor;
  colors[ImGuiCol_Separator] = borderColor;
  colors[ImGuiCol_SeparatorHovered] = borderColor;
  colors[ImGuiCol_SeparatorActive] = borderColor;
  colors[ImGuiCol_ResizeGrip] = bgColor;
  colors[ImGuiCol_ResizeGripHovered] = panelColor;
  colors[ImGuiCol_ResizeGripActive] = lightBgColor;
  colors[ImGuiCol_PlotLines] = panelActiveColor;
  colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
  colors[ImGuiCol_PlotHistogram] = panelActiveColor;
  colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
  colors[ImGuiCol_ModalWindowDarkening] = bgColor;
  colors[ImGuiCol_DragDropTarget] = bgColor;
  colors[ImGuiCol_NavHighlight] = bgColor;
  colors[ImGuiCol_DockingPreview] = panelActiveColor;
  colors[ImGuiCol_Tab] = bgColor;
  colors[ImGuiCol_TabActive] = panelActiveColor;
  colors[ImGuiCol_TabUnfocused] = bgColor;
  colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
  colors[ImGuiCol_TabHovered] = panelHoverColor;

  style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.PopupRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.TabRounding = 0.0f;
}

void ImGuiSys::Render()
{
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
}

} // namespace Octane
