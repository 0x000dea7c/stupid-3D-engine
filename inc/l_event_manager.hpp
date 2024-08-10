#pragma once

#include <functional>

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

struct event final {
  event_type _type;
};

namespace event_manager {

void Subscribe(event_type const type, std::function<void(event const&)> listener);

void Post(event const event);

}; // namespace event_manager

}; // namespace lain
