#include "Views.hpp"

#include <Allocation/sql_queries.hpp>


namespace Allocation::ServiceLayer::Views
{
    AllocationsView::AllocationsViewResponseBody Allocations(
        std::string orderid, userver::storages::postgres::ClusterPtr cluster)
    {
        auto response = cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
            sql::kSelectallocationsview, orderid);
        return response.AsContainer<AllocationsView::AllocationsViewResponseBody>();
    }
}