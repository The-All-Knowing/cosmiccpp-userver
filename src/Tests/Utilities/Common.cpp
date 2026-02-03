#include "Common.hpp"

#include <userver/utils/uuid7.hpp>


namespace Allocation::Tests
{
    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& sku, size_t batchQty, size_t lineQty)
    {
        return {Domain::Batch("batch-001", sku, batchQty),
            Domain::OrderLine("order-123", sku, lineQty)};
    }

    std::string RandomSuffix()
    {
        return userver::utils::generators::GenerateUuidV7();
    }

    std::string RandomSku(const std::string& name) { return "sku-" + name + "-" + RandomSuffix(); }

    std::string RandomBatchRef(const std::string& name)
    {
        return "batch-" + name + "-" + RandomSuffix();
    }

    std::string RandomOrderId(const std::string& name)
    {
        return "order-" + name + "-" + RandomSuffix();
    }
}