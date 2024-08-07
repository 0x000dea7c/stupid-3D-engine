#pragma once

namespace lain {

class main_menu_renderer;

class main_menu final {
public:
  main_menu(main_menu_renderer& mainMenuRenderer);

  void Render();

private:
  main_menu_renderer& _renderer;
};

}; // namespace lain
