#include "Events.hpp"


namespace Allocation::Domain
{

    IMessage::Type AbstractEvent::GetType() const
    {
        return IMessage::Type::Event;
    }

    Allocated::Allocated(const std::string& orderid, const std::string& sku, size_t qty,
            const std::string& batchref) : orderid(orderid), sku(sku), qty(qty), batchref(batchref)
    {
    }

    std::string Allocated::Name() const
    {
        return "Allocated"; 
    }

    Deallocated::Deallocated(const std::string& orderid, const std::string& sku, size_t qty)
        : orderid(orderid), sku(sku), qty(qty)
    {
    }

    std::string Deallocated::Name() const
    {
        return "Deallocated";
    }

    OutOfStock::OutOfStock(const std::string& sku) : sku(sku)
    {
    }

    std::string OutOfStock::Name() const
    {
        return "OutOfStock";
    }
}