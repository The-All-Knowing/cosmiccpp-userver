#include "SqlRepository.hpp"

#include <Allocation/sql_queries.hpp>


namespace Allocation::Adapters::Repository
{
    SqlRepository::SqlRepository(userver::storages::postgres::Transaction& transaction)
        : _transaction(transaction)
    {
    }

    void SqlRepository::Add(Domain::ProductPtr product)
    {
        _transaction.Execute(sql::kInsertproduct, product->GetSKU(), product->GetVersion());
        for (const auto& batch : product->GetBatches())
        {
            auto result = _transaction.Execute(sql::kInsertbatch, batch.GetReference(),
                product->GetSKU(), batch.GetPurchasedQuantity(), batch.GetETA());
            int batchId = result.AsSingleRow<int>();
            std::vector<std::tuple<int, std::string, int>> additionalOrderLines;
            for (const auto& orderLine : batch.GetAllocations())
                additionalOrderLines.emplace_back(batchId, orderLine.sku, orderLine.quantity);
            result = _transaction.Execute(sql::kInsertorderlines, additionalOrderLines);
            auto orderLineIds = result.AsContainer<std::vector<int>>();
            result = _transaction.Execute(sql::kInsertallocations, batchId, orderLineIds);
        }
    }

    void SqlRepository::Update(Domain::ProductPtr product)
    {
        auto batchRefs = product->GetModifiedBatches();
        std::vector<std::string> deletedBatchRefs;
        std::vector<std::string> addedBatchRefs;
        for (const auto& batchRef : batchRefs)
        {
            deletedBatchRefs.push_back(batchRef);
            if (product->GetBatch(batchRef).has_value())
                addedBatchRefs.push_back(batchRef);
        }
        auto result = _transaction.Execute(sql::kDeletebatch, deletedBatchRefs);
        auto deletedIds = result.AsContainer<std::vector<int>>();
        result = _transaction.Execute(sql::kDeleteallocations, deletedIds);
        auto orderLineIds = result.AsContainer<std::vector<int>>();
        _transaction.Execute(sql::kDeleteorderlines, orderLineIds);

        for (const auto& batchRef : addedBatchRefs)
        {
            const auto batch = product->GetBatch(batchRef).value();
            auto result = _transaction.Execute(sql::kInsertbatch, batch.GetReference(),
                product->GetSKU(), batch.GetPurchasedQuantity(), batch.GetETA());
            int batchId = result.AsSingleRow<int>();
            std::vector<std::tuple<int, std::string, int>> additionalOrderLines;
            for (const auto& orderLine : batch.GetAllocations())
                additionalOrderLines.emplace_back(batchId, orderLine.sku, orderLine.quantity);
            result = _transaction.Execute(sql::kInsertorderlines, additionalOrderLines);
            auto orderLineIds = result.AsContainer<std::vector<int>>();
            result = _transaction.Execute(sql::kInsertallocations, batchId, orderLineIds);
        }
    }

    Domain::ProductPtr SqlRepository::Get(const std::string& sku)
    {
        auto result = _transaction.Execute(sql::kSelectproductbysku, sku);
        auto productDTO = result.AsSingleRow<ProductDTO>();
        result = _transaction.Execute(sql::kSelectbatchesbysku, sku);
        auto batchesDTO = result.AsContainer<std::vector<BatchDTO>>();
        std::vector<int> batchIds;
        batchIds.reserve(batchesDTO.size());
        for (const auto& batch : batchesDTO)
            batchIds.push_back(batch.id);
        result = _transaction.Execute(sql::kSelectorderlinesbybatchpk, batchIds);
        auto orderLinesDTO = result.AsContainer<std::vector<OrderLineDTO>>();
        return MakeProduct(productDTO, batchesDTO, orderLinesDTO);
    }

    Domain::ProductPtr SqlRepository::GetByBatchRef(const std::string& batchRef)
    {
        auto result = _transaction.Execute(sql::kSelectproductbybatchref, batchRef);
        auto productDTO = result.AsSingleRow<ProductDTO>();
        result = _transaction.Execute(sql::kSelectbatchesbysku, productDTO.sku);
        auto batchesDTO = result.AsContainer<std::vector<BatchDTO>>();
        std::vector<int> batchIds;
        batchIds.reserve(batchesDTO.size());
        for (const auto& batch : batchesDTO)
            batchIds.push_back(batch.id);
        result = _transaction.Execute(sql::kSelectorderlinesbybatchpk, batchIds);
        auto orderLinesDTO = result.AsContainer<std::vector<OrderLineDTO>>();
        return MakeProduct(productDTO, batchesDTO, orderLinesDTO);
    }

    void SqlRepository::IncrementVersion(Domain::ProductPtr product)
    {
        _transaction.Execute(sql::kUpdateproductversion, product->GetSKU(), product->GetVersion());
    }

    Domain::ProductPtr SqlRepository::MakeProduct(const ProductDTO& product,
        const std::vector<BatchDTO>& batches, const std::vector<OrderLineDTO>& orderLines) const
    {
        std::unordered_map<int, std::vector<Domain::OrderLine>> batchIdByOrderLines;
        for (const auto& orderLine : orderLines)
            batchIdByOrderLines[orderLine.batchId].emplace_back(
                orderLine.sku, orderLine.qty, orderLine.orderid);

        std::vector<Domain::Batch> domainBatches;
        for (const auto& batch : batches)
            domainBatches.emplace_back(batch.reference, batch.sku, batch.purchased_quantity,
                batch.eta, batchIdByOrderLines[batch.id]);

        return std::make_shared<Domain::Product>(
            product.sku, domainBatches, product.version_number);
    }
}