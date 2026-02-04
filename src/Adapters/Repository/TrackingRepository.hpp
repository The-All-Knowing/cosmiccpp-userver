#pragma once

#include "Precompile.hpp"
#include "Domain/Ports/IRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief Репозиторий для отслеживания изменений агрегатов продуктов.
    class TrackingRepository final : public Domain::IRepository
    {
    public:
        /// @brief Конструктор.
        /// @param repo Отслеживаемый репозиторий.
        TrackingRepository(Domain::IRepository& repo);

        void Add(Domain::ProductPtr product) override;

        void Update(Domain::ProductPtr product) override;

        [[nodiscard]] Domain::ProductPtr Get(std::string_view sku) override;

        [[nodiscard]] Domain::ProductPtr GetByBatchRef(std::string_view batchRef) override;

        [[nodiscard]] std::unordered_set<Domain::ProductPtr> GetSeen() const noexcept;

        /// @brief Очищает все наблюдаемые продукты.
        void Clear() noexcept;

    private:
        Domain::IRepository& _repo;
        std::unordered_set<Domain::ProductPtr> _seenProducts;
    };
}