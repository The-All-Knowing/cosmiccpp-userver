/*
#pragma once

#include <userver/storages/redis/client.hpp>
#include <userver/storages/redis/component.hpp>

#include "Domain/Events/AbstractEvent.hpp"


namespace Allocation::Adapters::Redis
{
    /// @brief Публикует события в Redis.
    /// @tparam T Тип события.
    class RedisEventPublisher
    {
    public:
        /// @brief Конструктор.
        RedisEventPublisher(const userver::components::Redis& component, const std::string& channel,
            const userver::storages::redis::CommandControl& commandControl);

        template <typename T>
            requires std::derived_from<T, Domain::Events::AbstractEvent>
        void operator()(std::shared_ptr<T> event) const
        {
            _client->Publish(std::string channel, std::string message, const CommandControl &command_control, PubShard policy)
            /*
            for (const auto& [name, value] : GetAttributes<T>(event))
                json.set(name, value);

            std::stringstream ss;
            json.stringify(ss);
            Poco::Redis::Command publish("PUBLISH");
            publish << channel << ss.str();

            try
            {
                static_cast<Poco::Redis::Client::Ptr>(_connection)->execute<Poco::Int64>(publish);
            }
            catch (const Poco::Exception& e)
            {
                Allocation::Loggers::GetLogger()->Error(
                    std::format("Redis publish failed: {}", e.displayText()));
            }
            catch (...)
            {
                Allocation::Loggers::GetLogger()->Error("Redis publish failed: unknown error");
            }

        }

    private:
        userver::storages::redis::ClientPtr _client;
        userver::storages::redis::CommandControl _commandControl;
    };
}
*/