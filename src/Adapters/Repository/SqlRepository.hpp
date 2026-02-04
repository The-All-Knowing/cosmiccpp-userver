#pragma once

#include "Precompile.hpp"

#include <userver/storages/postgres/transaction.hpp>

#include "DTO.hpp"
#include "Domain/Ports/IRepository.hpp"



namespace Allocation::Adapters::Repository
{
    /// @brief Реализация репозитория для работы с PostgreSQL СУБД.
    class SqlRepository final : public Domain::IRepository
    {
    public:
        explicit SqlRepository(userver::storages::postgres::Transaction& transaction);

        void Add(Domain::ProductPtr product) override;

        void Update(Domain::ProductPtr product) override;

        [[nodiscard]] Domain::ProductPtr Get(std::string_view sku) override;

        [[nodiscard]] Domain::ProductPtr GetByBatchRef(std::string_view batchRef) override;

        void IncrementVersion(Domain::ProductPtr product);

    private:
        std::vector<Domain::OrderLine> MakeOrderLines(const std::vector<DTO::OrderLineRow>& orderLineRows) const;

        std::vector<Domain::Batch> MakeBatches(const std::vector<DTO::BatchRow>& batchRows) const;

        Domain::ProductPtr MakeProduct(
            const DTO::ProductRow& productRow, const std::vector<Domain::Batch>& batches) const;

        std::unordered_map<std::string, std::pair<Domain::ProductPtr, size_t>> _skuToProductAndInitVersion;
        userver::storages::postgres::Transaction& _transaction;
    };
}