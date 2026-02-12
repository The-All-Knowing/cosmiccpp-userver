#pragma once

#include "Domain/Ports/IRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief Репозиторий для отслеживания изменений агрегатов продуктов.
    class TrackingRepository final : public Domain::IRepository
    {
    public:
        /// @brief Конструктор.
        /// @param repo Отслеживаемый репозиторий.
        TrackingRepository(Domain::ICommonRepository& repo);

        void Add(Domain::ProductPtr product) override;

        [[nodiscard]] Domain::ProductPtr Get(const std::string& sku) override;

        [[nodiscard]] Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override;

        [[nodiscard]] std::vector<Domain::ProductPtr> GetSeen() const noexcept;

        void Clear() noexcept;

    private:
        Domain::ICommonRepository& _repo;
        std::unordered_map<std::string, Domain::ProductPtr> _seenProducts;
    };
}