#pragma once

#include "l_renderer.hpp"
namespace lain {

class event_manager;

class main_menu_renderer : public renderer {
public:
  main_menu_renderer(float const width, float const height, event_manager& eventManager);

  void RenderMainMenu();

private:
  event_manager& _eventManager;
};

}; // namespace lain
