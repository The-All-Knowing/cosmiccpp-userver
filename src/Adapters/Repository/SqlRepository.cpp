#include "SqlRepository.hpp"

#include <Allocation/sql_queries.hpp>
#include <userver/storages/postgres/io/date.hpp>


namespace Allocation::Adapters::Repository
{
    SqlRepository::SqlRepository(userver::storages::postgres::Transaction& transaction)
        : _transaction(transaction)
    {
    }

    void SqlRepository::Flush(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("Product is null");

        if (const auto it = _loadedProducts.find(product->GetSKU()); it == _loadedProducts.end())
            insert(product);
        else if (product->GetVersion() != it->second)
            update(product);
    }

    void SqlRepository::update(Domain::ProductPtr product)
    {
        auto sku = product->GetSKU();
        auto respond = _transaction.Execute(sql::kDeletebatches, sku);
        auto batchIds = respond.AsContainer<std::vector<int>>();
        respond = _transaction.Execute(sql::kDeleteorderlines, batchIds);
        auto orderLineIds = respond.AsContainer<std::vector<int>>();
        respond = _transaction.Execute(sql::kDeleteallocations, batchIds, orderLineIds);
        _transaction.Execute(sql::kUpdateproductversion, sku, product->GetVersion());
        insertBatches(product->GetBatches());
        _loadedProducts[sku] = product->GetVersion();
    }

    void SqlRepository::insert(Domain::ProductPtr product)
    {
        auto sku = product->GetSKU();
        insertBatches(product->GetBatches());
        _transaction.Execute(sql::kInsertproduct, sku, product->GetVersion());
        _loadedProducts.emplace(sku, product->GetVersion());
    }

    Domain::ProductPtr SqlRepository::Get(const std::string& sku)
    {
        if (sku.empty())
            throw std::invalid_argument("SKU is empty");

        auto respond = _transaction.Execute(sql::kSelectproductbysku, sku);
        auto product = respond.AsSingleRow<ProductDTO>();
        std::vector<BatchDTO> batches;
        std::vector<OrderLineDTO> orderLines;
        getBatchesAndOrderLines(product.sku, batches, orderLines);
        auto result = makeProduct(product, batches, orderLines);
        _loadedProducts.emplace(sku, result->GetVersion());
        return result;
    }

    Domain::ProductPtr SqlRepository::GetByBatchRef(const std::string& batchRef)
    {
        if (batchRef.empty())
            throw std::invalid_argument("Batch reference is empty");

        auto respond = _transaction.Execute(sql::kSelectproductbybatchref, batchRef);
        auto product = respond.AsSingleRow<ProductDTO>();
        std::vector<BatchDTO> batches;
        std::vector<OrderLineDTO> orderLines;
        getBatchesAndOrderLines(product.sku, batches, orderLines);
        auto result = makeProduct(product, batches, orderLines);
        _loadedProducts.emplace(result->GetSKU(), result->GetVersion());
        return result;
    }

    void SqlRepository::getBatchesAndOrderLines(const std::string& sku,
        std::vector<BatchDTO>& batches, std::vector<OrderLineDTO>& orderLines) const
    {
        auto respond = _transaction.Execute(sql::kSelectbatchesbysku, sku);
        batches = respond.AsContainer<std::vector<BatchDTO>>();
        std::vector<int> batchIds;
        batchIds.reserve(batches.size());
        for (const auto& batch : batches)
            batchIds.push_back(batch.id);
        respond = _transaction.Execute(sql::kSelectorderlinesbybatchpk, batchIds);
        orderLines = respond.AsContainer<std::vector<OrderLineDTO>>();
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
            auto respond = _transaction.Execute(sql::kInsertbatch, batch.GetReference(),
                batch.GetSKU(), static_cast<std::int32_t>(batch.GetPurchasedQuantity()), eta);
            int batchId = respond.AsSingleRow<int>();
            auto orderLines = batch.GetAllocations();
            std::vector<int> quantities;
            quantities.reserve(orderLines.size());
            for (const auto& orderLine : orderLines)
                quantities.push_back(orderLine.quantity);
            respond =
                _transaction.Execute(sql::kInsertorderlines, batchId, batch.GetSKU(), quantities);
            auto orderLineIds = respond.AsContainer<std::vector<int>>();
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