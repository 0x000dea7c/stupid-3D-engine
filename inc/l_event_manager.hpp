#pragma once

#include "l_event.hpp"
#include <functional>

namespace lain {

namespace event_manager {

void Subscribe(event_type const type, std::function<void(event const&)> listener);

void Post(event const event);

}; // namespace event_manager

}; // namespace lain
