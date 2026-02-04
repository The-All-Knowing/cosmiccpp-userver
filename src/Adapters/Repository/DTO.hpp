#pragma once

#include "Precompile.hpp"
#include <userver/utils/datetime/date.hpp>

namespace Allocation::Adapters::Repository::DTO
{

    struct OrderLineRow final
    {
        std::string sku;
        int qty;
        std::string orderid;
    };

    struct BatchRow final
    {
        int id;
        std::string reference;
        std::string sku;
        int purchased_quantity;
        std::optional<userver::utils::datetime::Date> eta;
    };

    struct ProductRow final
    {
        std::string sku;
        size_t version_number;
    };
}