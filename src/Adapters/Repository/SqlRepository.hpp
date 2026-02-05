#pragma once

#include "Precompile.hpp"

#include <userver/storages/postgres/transaction.hpp>
#include <userver/utils/datetime/date.hpp>

#include "Domain/Ports/IRepository.hpp"



namespace Allocation::Adapters::Repository
{
    /// @brief Реализация репозитория для работы с PostgreSQL СУБД.
    class SqlRepository final : public Domain::IRepository
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
            size_t version_number;
        };

    public:
        explicit SqlRepository(userver::storages::postgres::Transaction& transaction);

        void Add(Domain::ProductPtr product) override;

        void Update(Domain::ProductPtr product) override;

        [[nodiscard]] Domain::ProductPtr Get(const std::string& sku) override;

        [[nodiscard]] Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override;

        void IncrementVersion(Domain::ProductPtr product);

    private:
        Domain::ProductPtr MakeProduct(const ProductDTO& product,
            const std::vector<BatchDTO>& batches,
            const std::vector<OrderLineDTO>& orderLines) const;

        userver::storages::postgres::Transaction& _transaction;
    };
}