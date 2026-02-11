#include "SqlRepository.hpp"

#include <Allocation/sql_queries.hpp>
#include <userver/storages/postgres/io/date.hpp>


namespace Allocation::Adapters::Repository
{
    SqlRepository::SqlRepository(userver::storages::postgres::Transaction& transaction)
        : _transaction(transaction)
    {
    }

    void SqlRepository::Add(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("Product is null");

        auto sku = product->GetSKU();
        _transaction.Execute(
            sql::kInsertproduct, sku, static_cast<std::int64_t>(product->GetVersion()));
        insertBatches(product->GetBatches());
    }

    void SqlRepository::Update(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("Product is null");

        auto sku = product->GetSKU();
        auto result = _transaction.Execute(sql::kDeletebatches, sku);
        auto batchIds = result.AsContainer<std::vector<int>>();
        result = _transaction.Execute(sql::kDeleteorderlines, batchIds);
        auto orderLineIds = result.AsContainer<std::vector<int>>();
        result = _transaction.Execute(sql::kDeleteallocations, batchIds, orderLineIds);
        insertBatches(product->GetBatches());
    }

    Domain::ProductPtr SqlRepository::Get(const std::string& sku)
    {
        if (sku.empty())
            throw std::invalid_argument("SKU is empty");

        auto result = _transaction.Execute(sql::kSelectproductbysku, sku);
        auto product = result.AsSingleRow<ProductDTO>();
        std::vector<BatchDTO> batches;
        std::vector<OrderLineDTO> orderLines;
        getBatchesAndOrderLines(product.sku, batches, orderLines);
        return makeProduct(product, batches, orderLines);
    }

    Domain::ProductPtr SqlRepository::GetByBatchRef(const std::string& batchRef)
    {
        if (batchRef.empty())
            throw std::invalid_argument("Batch reference is empty");

        auto result = _transaction.Execute(sql::kSelectproductbybatchref, batchRef);
        auto product = result.AsSingleRow<ProductDTO>();
        std::vector<BatchDTO> batches;
        std::vector<OrderLineDTO> orderLines;
        getBatchesAndOrderLines(product.sku, batches, orderLines);
        return makeProduct(product, batches, orderLines);
    }

    void SqlRepository::IncrementVersion(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("Product is null");

        _transaction.Execute(sql::kUpdateproductversion, product->GetSKU(),
            static_cast<std::int64_t>(product->GetVersion()));
    }

    void SqlRepository::getBatchesAndOrderLines(const std::string& sku,
        std::vector<BatchDTO>& batches, std::vector<OrderLineDTO>& orderLines) const
    {
        auto result = _transaction.Execute(sql::kSelectbatchesbysku, sku);
        batches = result.AsContainer<std::vector<BatchDTO>>();
        std::vector<int> batchIds;
        batchIds.reserve(batches.size());
        for (const auto& batch : batches)
            batchIds.push_back(batch.id);
        result = _transaction.Execute(sql::kSelectorderlinesbybatchpk, batchIds);
        orderLines = result.AsContainer<std::vector<OrderLineDTO>>();
    }

    void SqlRepository::insertBatches(const std::vector<Domain::Batch>& batches)
    {
        for (const auto& batch : batches)
        {
            std::optional<userver::utils::datetime::Date> eta;
            if (const auto& src = batch.GetETA())
            {
                std::chrono::sys_days days = std::chrono::year{src->year()} /
                                             std::chrono::month{src->month()} /
                                             std::chrono::day{src->day()};
                eta = userver::utils::datetime::Date(days);
            }
            auto result = _transaction.Execute(sql::kInsertbatch, batch.GetReference(),
                batch.GetSKU(), static_cast<std::int32_t>(batch.GetPurchasedQuantity()), eta);
            int batchId = result.AsSingleRow<int>();
            auto orderLines = batch.GetAllocations();
            std::vector<int> quantities;
            quantities.reserve(orderLines.size());
            for (const auto& orderLine : orderLines)
                quantities.push_back(orderLine.quantity);
            result =
                _transaction.Execute(sql::kInsertorderlines, batchId, batch.GetSKU(), quantities);
            auto orderLineIds = result.AsContainer<std::vector<int>>();
            _transaction.Execute(sql::kInsertallocations, batchId, orderLineIds);
        }
    }

    Domain::ProductPtr SqlRepository::makeProduct(const ProductDTO& product,
        const std::vector<BatchDTO>& batches, const std::vector<OrderLineDTO>& orderLines) const
    {
        std::unordered_map<int, std::vector<Domain::OrderLine>> batchIdByOrderLines;
        for (const auto& ol : orderLines)
        {
            auto& lines = batchIdByOrderLines.try_emplace(ol.batchId).first->second;
            lines.emplace_back(ol.sku, ol.orderid, ol.qty);
        }

        std::vector<Domain::Batch> domainBatches;
        for (const auto& batch : batches)
        {
            std::optional<std::chrono::year_month_day> eta;
            if (const auto& src = batch.eta)
                eta = std::chrono::year_month_day{src->GetSysDays()};
            auto& newBatch = domainBatches.emplace_back(
                batch.reference, batch.sku, static_cast<size_t>(batch.purchased_quantity), eta);
            if (const auto it = batchIdByOrderLines.find(batch.id); it != batchIdByOrderLines.end())
                for (const auto& orderLine : it->second)
                    newBatch.Allocate(orderLine);
        }
        return std::make_shared<Domain::Product>(
            product.sku, domainBatches, product.version_number);
    }
}