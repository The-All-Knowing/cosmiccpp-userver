#pragma once

#include "Domain/Ports/IRepository.hpp"
#include "Domain/Product/Product.hpp"


namespace Allocation::Tests
{
    /// @brief Фейковый репозиторий для тестирования.
    class FakeRepository final : public Domain::IRepository
    {
    public:
        /// @brief Конструктор.
        FakeRepository() = default;

        /// @brief Конструктор.
        /// @param init Инициализирующий список продуктов.
        FakeRepository(const std::vector<Domain::ProductPtr>& init)
        {
            for (const auto& prod : init)
                _skuByProduct.emplace(prod->GetSKU(), prod);
        }

        /// @brief Добавляет или обновляет продукт в репозиторий.
        /// @param product Продукт для добавления.
        void Add(Domain::ProductPtr product) override
        {
            const auto result = _skuByProduct.emplace(product->GetSKU(), product);
            if (!result.second)
                throw std::invalid_argument("Product with the same SKU already exists");
        }

        /// @brief Получает продукт из репозитория.
        /// @param sku Артикул продукта.
        /// @return Продукт с заданным артикулом.
        [[nodiscard]] Domain::ProductPtr Get(const std::string& sku) override
        {
            if (const auto it = _skuByProduct.find(sku); it != _skuByProduct.end())
                return it->second;
            return nullptr;
        }

        /// @brief Получает продукт из репозитория по ссылке партии.
        /// @param batchRef Ссылка на партию.
        /// @return Продукт с заданной ссылкой партии.
        [[nodiscard]] virtual Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override
        {
            for (const auto& [_, product] : _skuByProduct)
                if (product->GetBatch(batchRef).has_value())
                    return product;
            return nullptr;
        }

        virtual void Update(Domain::ProductPtr product) override
        {
            _skuByProduct.insert_or_assign(product->GetSKU(), product);
        }

        std::unordered_map<std::string, Domain::ProductPtr> _skuByProduct;
    };
}