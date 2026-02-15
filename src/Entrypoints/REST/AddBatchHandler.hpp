#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>


namespace Allocation::Entrypoints::Rest
{
    class AddBatchHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
    public:
        static constexpr std::string_view kName = "AddBatchHandler";

        AddBatchHandler(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

        userver::formats::json::Value HandleRequestJsonThrow(
            const userver::server::http::HttpRequest& request,
            const userver::formats::json::Value& json,
            userver::server::request::RequestContext&) const override;

    private:
        userver::storages::postgres::ClusterPtr _pgCluster;
    };
}