#include "l_event_manager.hpp"

namespace lain {

namespace event_manager {

static std::unordered_map<event_type, std::vector<std::function<void(event const&)>>> _listeners;

void Subscribe(event_type const type, std::function<void(event const&)> listener) {
  _listeners[type].push_back(listener);
}

void Post(event const event) {
  auto& listeners = _listeners[event.GetType()];

  for (auto const& f : listeners) {
    f(event);
  }
}

}; // namespace event_manager

}; // namespace lain
