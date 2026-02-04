#include "SqlRepository.hpp"

#include <Allocation/sql_queries.hpp>

#include "DTO.hpp"



namespace Allocation::Adapters::Repository
{
    SqlRepository::SqlRepository(userver::storages::postgres::Transaction& transaction)
        : _transaction(transaction)
    {
    }

    void SqlRepository::Add(Domain::ProductPtr product)
    {

    }

    void SqlRepository::Update(Domain::ProductPtr product) {}

    Domain::ProductPtr SqlRepository::Get(std::string_view sku)
    {
        if (auto it = _skuToProductAndInitVersion.find(std::string(sku));
            it != _skuToProductAndInitVersion.end())
            return it->second.first;

        auto result = _transaction.Execute(sql::kGetProductBySku, sku);
        auto productRow = result.AsSingleRow<DTO::ProductRow>();
        result = _transaction.Execute(sql::kGetBatchesBySku, sku);
        auto batchRows = result.AsVector<DTO::BatchRow>();

        return _skuToProductAndInitVersion
            .emplace(sku, std::make_pair(MakeProduct(productRow, MakeBatches(batchRows)),
                              productRow.version_number))
            .first->second.first;
    }

    Domain::ProductPtr SqlRepository::GetByBatchRef(std::string_view batchRef)
    {
        for (const auto& [_, productAndVersion] : _skuToProductAndInitVersion)
            if (productAndVersion.first->GetBatch(std::string(batchRef)))
                return productAndVersion.first;

        auto result = _transaction.Execute(Allocation::sql_queries::GetProductByBatchRef, batchRef);
        auto productRow = result.AsSingleRow<DTO::ProductRow>();
        result = _transaction.Execute(Allocation::sql_queries::GetBatchesByBatchRef, batchRef);
        auto batchRows = result.AsVector<DTO::BatchRow>();

        return _skuToProductAndInitVersion
            .emplace(productRow.sku, std::make_pair(MakeProduct(productRow, MakeBatches(batchRows)),
                                         productRow.version_number))
            .first->second.first;
    }

    std::vector<Domain::OrderLine> SqlRepository::MakeOrderLines(
        const std::vector<DTO::OrderLineRow>& orderLineRows) const
    {
        std::vector<Domain::OrderLine> orderLines;
        orderLines.reserve(orderLineRows.size());
        for (const auto& row : orderLineRows)
            orderLines.emplace_back(row.sku, row.qty, row.orderid);
        return orderLines;
    }

    std::vector<Domain::Batch> SqlRepository::MakeBatches(
        const std::vector<DTO::BatchRow>& batchRows) const
    {
        std::vector<Domain::Batch> batches;
        batches.reserve(batchRows.size());
        for (const auto& batchRow : batchRows)
        {
            auto orderLineRows =
                _transaction.Execute(Allocation::sql_queries::GetOrderLinesByBatchId, batchRow.id);
            auto orderLines = MakeOrderLines(orderLineRows);
            batches.emplace_back(batchRow.reference, batchRow.sku, batchRow.purchased_quantity,
                batchRow.eta, orderLines);
        }
        return batches;
    }

    Domain::ProductPtr SqlRepository::MakeProduct(
        const DTO::ProductRow& productRow, const std::vector<Domain::Batch>& batches) const
    {
        return std::make_shared<Domain::Product>(
            productRow.sku, batches, productRow.version_number);
    }
}