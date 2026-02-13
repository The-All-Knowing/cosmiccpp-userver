#pragma once

#include "Domain/Product/Product.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс репозитория для работы с агрегатами-продуктами в хранилище.
    class ICommonRepository
    {
    public:
        ICommonRepository() = default;    

        virtual ~ICommonRepository() = default;

        [[nodiscard]] virtual Domain::ProductPtr Get(const std::string& sku) = 0;

        [[nodiscard]] virtual Domain::ProductPtr GetByBatchRef(const std::string& batchRef) = 0;

    private:
        ICommonRepository(const ICommonRepository&) = delete;
        ICommonRepository& operator=(const ICommonRepository&) = delete;
    };

    class IRepository : public ICommonRepository
    {
    public:
        virtual void Add(Domain::ProductPtr product) = 0;
    };
}