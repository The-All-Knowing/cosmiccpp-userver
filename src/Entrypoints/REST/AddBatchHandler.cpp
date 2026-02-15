#include "AddBatchHandler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>

#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "schemas/AddBatch_fwd.hpp"


namespace Allocation::Entrypoints::Rest
{
    AddBatchHandler::AddBatchHandler(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
        : HttpHandlerJsonBase(config, context),
          _pgCluster(context.FindComponent<userver::components::Postgres>("key-value-database")
                         .GetCluster())
    {
    }

    userver::formats::json::Value AddBatchHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& json, userver::server::request::RequestContext&) const
    {
        try
        {
            auto requestDto = json.As<AddBatch::CreateBatch>();
            ServiceLayer::UoW::SqlUnitOfWork uow(_pgCluster);
            ServiceLayer::MessageBus::Instance().HandleMessage(requestDto, uow);

            request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kCreated);
            request.GetHttpResponse().SetContentType(userver::http::content_type::kApplicationJson);
            userver::formats::json::ValueBuilder result;
            result["message"] = "Batch added";
            return result.ExtractValue();
        }
        catch (const std::exception& ex)
        {
            request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kConflict);
            return userver::formats::json::MakeObject("error", ex.what());
        }
    }
}