#pragma once

#include <userver/storages/postgres/cluster.hpp>

#include "schemas/AllocationsView.hpp"


namespace Allocation::ServiceLayer::Views
{
    AllocationsView::AllocationsViewResponseBody Allocations(
        const std::string& orderid, userver::storages::postgres::ClusterPtr cluster);
}