#pragma once

#include "l_event.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

namespace lain {

class event_manager final {
public:
  void Subscribe(event_type const type, std::function<void(event const&)> listener) {
    _listeners[type].push_back(listener);
  }

  void Post(event const event) {
    auto& listeners = _listeners[event.GetType()];

    for (auto const& f : listeners) {
      f(event);
    }
  }

private:
  std::unordered_map<event_type, std::vector<std::function<void(event const&)>>> _listeners;
};

}; // namespace lain
