#include "MessageBusComponent.hpp"

#include <userver/components/component_config.hpp>
//#include <userver/yaml_config/yaml_config.hpp>
#include <userver/yaml_config/merge_schemas.hpp>


namespace Allocation::Adapters::MessageBus
{

    MessageBusComponent::MessageBusComponent(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
        : userver::components::ComponentBase(config, context)
    {
        const auto& handlers = config["event-handlers"];

        for (const auto& h : handlers)
        {
            const auto type = h["type"].As<std::string>();
            const auto handler = h["handler"].As<std::string>();
        }
    }


    userver::yaml_config::Schema MessageBusComponent::GetStaticConfigSchema()
    {
        return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
type: object
description: user component smth
additionalProperties: false
properties:
  command-handlers:
    type: array
    description: user component smth
    items:
      type: object
      description: user component smth
      additionalProperties: false
      properties:
        type:
          type: string
          description: user component smth
        handler:
          type: string
          description: user component smth
        task-processor:
          type: string
          description: user component smth
  event-handlers:
    type: array
    description: user component smth
    items:
      type: object
      description: user component smth
      additionalProperties: false
      properties:
        type:
          type: string
          description: user component smth
        handler:
          type: string
          description: user component smth
        task-processor:
          type: string
          description: user component smth
  queue-size:
    type: integer
    description: user component smth
    default: 1000
  timeout-sec:
    type: integer
    description: user component smth
    default: 30
)");
    }
}