#include "Commands.hpp"


namespace Allocation::Domain
{

    IMessage::Type AbstractCommand::GetType() const { return Type::Command; }

    Allocate::Allocate(const std::string& orderid, const std::string& sku, size_t qty)
        : orderid(orderid), sku(sku), qty(qty)
    {
    }

    std::string Allocate::Name() const { return "Allocate"; }

    ChangeBatchQuantity::ChangeBatchQuantity(const std::string& ref, size_t qty)
        : ref(ref), qty(qty)
    {
    }

    std::string ChangeBatchQuantity::Name() const { return "ChangeBatchQuantity"; }

    CreateBatch::CreateBatch(const std::string& ref, const std::string& sku, size_t qty,
        std::optional<std::chrono::year_month_day> eta)
        : ref(ref), sku(sku), qty(qty), eta(eta)
    {
    }

    std::string CreateBatch::Name() const { return "CreateBatch"; }
}