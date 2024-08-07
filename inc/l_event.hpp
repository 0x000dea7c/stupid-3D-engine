#pragma once

namespace lain {

enum class event_type {
  main_menu_click_play,
  main_menu_click_level_editor,
  main_menu_click_options,
  main_menu_click_quit,

  level_editor_spawn_ball,
  level_editor_spawn_maze,
  level_editor_clear,
};

class event final {
public:
  event(event_type type)
      : _type{type} {}

  event_type GetType() const { return _type; }

private:
  event_type _type;
};

}; // namespace lain
