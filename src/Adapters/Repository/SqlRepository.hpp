#pragma once

#include <userver/storages/postgres/transaction.hpp>
#include <userver/utils/datetime/date.hpp>

#include "Domain/Ports/IRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief Реализация репозитория для работы с PostgreSQL СУБД.
    class SqlRepository final : public Domain::ICommonRepository
    {
        struct OrderLineDTO final
        {
            int batchId;
            std::string sku;
            int qty;
            std::string orderid;
        };

        struct BatchDTO final
        {
            int id;
            std::string reference;
            std::string sku;
            int purchased_quantity;
            std::optional<userver::utils::datetime::Date> eta;
        };

        struct ProductDTO final
        {
            std::string sku;
            int64_t version_number;
        };

    public:
        explicit SqlRepository(userver::storages::postgres::Transaction& transaction);

        [[nodiscard]] Domain::ProductPtr Get(const std::string& sku) override;

        [[nodiscard]] Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override;

        void Flush(Domain::ProductPtr product);

    private:
        void update(Domain::ProductPtr product);

        void insert(Domain::ProductPtr product);

        void getBatchesAndOrderLines(const std::string& sku, std::vector<BatchDTO>& batches,
            std::vector<OrderLineDTO>& orderLines) const;

        void insertBatches(const std::vector<Domain::Batch>& batches);

        Domain::ProductPtr makeProduct(const ProductDTO& product,
            const std::vector<BatchDTO>& batches,
            const std::vector<OrderLineDTO>& orderLines) const;

        std::unordered_map<std::string, int64_t> _loadedProducts;
        userver::storages::postgres::Transaction& _transaction;
    };
}