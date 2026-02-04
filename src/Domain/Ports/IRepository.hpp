#pragma once

#include "Precompile.hpp"
#include "Domain/Product/Product.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс репозитория для работы с агрегатами-продуктами в хранилище.
    class IRepository
    {
    public:
        /// @brief Конструктор.
        IRepository() = default;

        /// @brief Деструктор.
        virtual ~IRepository() = default;
        
        void virtual Add(Domain::ProductPtr product) = 0;

        void virtual Update(Domain::ProductPtr product) = 0;

        [[nodiscard]] virtual Domain::ProductPtr Get(std::string_view sku) = 0;

        /// @brief Возвращает агрегат-продукт по идентификатору партии включённого в него.
        /// @param batchRef Ссылка на партию поставки.
        /// @return Агрегат-продукт, если найден, иначе nullptr.
        [[nodiscard]] virtual Domain::ProductPtr GetByBatchRef(std::string_view batchRef) = 0;

    private:
        /// @brief Удалённый конструктор копирования.
        IRepository(const IRepository&) = delete;

        /// @brief Удалённый конструктор перемещения.
        IRepository(IRepository&&) = delete;

        /// @brief Удалённый оператор присвоения копирования.
        IRepository& operator=(const IRepository&) = delete;

        /// @brief Удалённый оператор присвоения перемещения.
        IRepository& operator=(IRepository&&) = delete;
    };
}