#pragma once

#include <userver/components/component_base.hpp>
#include <ServiceLayer/MessageBus/MessageBus.hpp>


namespace Allocation::Adapters::MessageBus
{
    class MessageBusComponent final : public userver::components::ComponentBase
    {
    public:
        static constexpr std::string_view kName = "message-bus-adapter";

        MessageBusComponent(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

        static userver::yaml_config::Schema GetStaticConfigSchema();

        ServiceLayer::MessageBus& GetMessageBus() noexcept;

    private:
        ServiceLayer::MessageBus _bus;
    };
}