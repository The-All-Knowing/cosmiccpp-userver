/*
#include "RedisEventPublisher.hpp"


namespace Allocation::Adapters::Redis
{
    RedisEventPublisher::RedisEventPublisher(const userver::components::Redis& component,
        const std::string& channel, const userver::storages::redis::CommandControl& commandControl)
        : _commandControl(commandControl)
    {
        _client = component.GetClient(channel);
    }
}
*/