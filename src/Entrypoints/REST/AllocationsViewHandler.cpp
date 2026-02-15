#include "AllocationsViewHandler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/formats/serialize/common_containers.hpp>

#include "ServiceLayer/Views.hpp"


namespace Allocation::Entrypoints::Rest
{
    AllocationsViewHandler::AllocationsViewHandler(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
        : HttpHandlerJsonBase(config, context),
          _pgCluster(context.FindComponent<userver::components::Postgres>("key-value-database")
                         .GetCluster())
    {
    }

    userver::formats::json::Value AllocationsViewHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& json, userver::server::request::RequestContext&) const
    {
        try
        {
            auto orderid = json["orderid"].As<std::string>();
            auto results = ServiceLayer::Views::Allocations(orderid, _pgCluster);
            request.GetHttpResponse().SetStatusOk();
            request.GetHttpResponse().SetContentType(userver::http::content_type::kApplicationJson);
            return userver::formats::json::ValueBuilder{results}.ExtractValue();
        }
        catch (const std::exception& ex)
        {
            request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kConflict);
            return userver::formats::json::MakeObject("error", ex.what());
        }
    }
}