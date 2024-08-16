#include "l_event_manager.h"

namespace lain
{
    namespace event_manager
    {
        static std::unordered_map<event_type, std::vector<std::function<void(event const&)>>> _listeners;

        void Subscribe(event_type const type, std::function<void(event const&)> listener)
        {
            _listeners[type].push_back(listener);
        }

        void Post(event const event)
        {
            auto& listeners = _listeners[event._type];

            for (auto const& f : listeners) {
                f(event);
            }
        }
    };
};
