#include "l_main_menu.hpp"
#include "l_main_menu_renderer.hpp"

namespace lain {

main_menu::main_menu(main_menu_renderer& mainMenuRenderer)
    : _renderer{mainMenuRenderer} {}

void main_menu::Render() { _renderer.RenderMainMenu(); }

}; // namespace lain
